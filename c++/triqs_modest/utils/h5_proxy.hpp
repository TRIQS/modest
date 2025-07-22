#pragma once
#include <cassert>
#include <ranges>
#include <memory>
#include <type_traits>
#include <utility>
#include <triqs/utility/generator.hpp>
#include <h5/h5.hpp>

// mv to cpp
#include <fmt/core.h>
// We need to move the IS function into the h5
#include <hdf5.h>

namespace h5 {

  using dcomplex = std::complex<double>;

  // It is a proxy to an h5::file, with the same interface as h5::file
  // A proxy to an element of the h5 tree, i.e.
  // its parent group and key string.
  // It can be a dataset, a group: its kind is determined at runtime
  class proxy {
    group g;
    std::string _key;
    std::string g_path; // for error messages
    char mode;          // same as h5::file

    proxy(group g, std::string const &key, std::string const &g_path, char mode)
       : g{std::move(g)}, _key{key}, g_path{g_path}, mode{mode} {}

    public:
    friend std::ostream &operator<<(std::ostream &out, proxy const &p) {
      return out << fmt::format("h5 proxy {} : path = {}, key ={}.   ", (p.is_group() ? "[Group]" : "Dataset"),
                                p.g_path, p._key);
    }

    // I would prefer to construct from file, but I can not access mode from file.
    // Fix it ?
    // Same constructor as h5::file
    // Ideally, add operator [string] to h5::file --> proxy
    // then we can write f = h5::file(...);   auto root = f["/"];
    proxy(std::string const &name, char mode) : g{file(name, mode)}, _key{"/"}, mode{mode} {}

    // Do we need this accessor ?
    //[[nodiscard]] std::string const &key() const { return _key; }

    private: // do we need it public ??
    // Full path (group + key)
    [[nodiscard]] std::string path() const { return g_path + "/" + _key; }

    public:
    // ======================= group related functions ==========================

    /// Does the proxy point to a subgroup ?
    [[nodiscard]] bool is_group() const { return g.has_subgroup(_key); }

    /// Does the proxy contain the subgroup name
    [[nodiscard]] bool has_group(const std::string &name) const { return g.has_subgroup(name); }

    // Assert is_group() is true or throw
    void assert_is_group() const {
      if (not is_group()) {
        std::cerr << fmt::format("{} is not a group", g_path) << std::endl;
        throw std::runtime_error{fmt::format("{} is not a group", g_path)};
      }
    }

    // Returns h5::group corresponding to the location.
    // throws if it is not a group.
    // In writing mode, create the group if necessary
    operator group() const {
      assert_is_group();
      if ((mode == 'r') or (g.has_key(_key))) return g.open_group(_key);
      return g.create_group(_key);
    }

    // ======================= The [] operator ==========================

    // Creates a proxy on a element of this.
    // Throws if this does not point to a group.
    // If 'r' mode : check that the key actually exits.
    //
    proxy operator[](std::string const &key) const {
      auto gg = group(*this);
      if (!gg.has_key(key)) throw std::runtime_error{fmt::format("{} is not a key of {}", key, g_path)};
      return {std::move(gg), key, path(), mode};
    }

    // ======================= READING from the proxy ==========================

    // as<T>(g["a"]) read the dataset at the location pointed to by g["a"]
    // as a T
    template <typename T> [[nodiscard]] T as() const {
      static_assert(not std::is_reference_v<T>); //
      if constexpr (std::is_same_v<T, group>)    // just in case, for generic code ? as<group>(X) is group(X)
        return group(*this);
      else
        return ::h5::read<T>(g, _key); // :: full qualify just to be sure, read is quite common...
    }

    // Keep both ? It is UFC !
    // as<T>(g["a"]) is equivalent to h5::read<T>(g, "a");
    template <typename T> [[nodiscard]] friend T as(proxy const &x) { return x.as<T>(); }

    //
    template <typename T> friend bool is(proxy const &x) {
      dataset ds  = x.g.open_dataset(x._key);
      datatype ty = H5Dget_type(ds);
      return ty == h5::detail::hid_t_of<T>();
    }

    // x << g["a"] is equivalent to h5_read(g, "a", x);
    //
    template <typename T> friend void operator<<(T &&lhs, proxy const &x) { ::h5::read(x.g, x._key, lhs); }

    // ======================= Writing from the proxy ==========================

    // Writing
    // g["a"] = X is equivalent to h5::write(g, "a", X);
    // throws in 'r' mode
    proxy &operator<<(auto &&x) {
      if (mode == 'r') throw std::runtime_error{"Read only file. Can not write."};
      ::h5::write(g, _key, x);
    }

    // Predefined cast operators
    // Keep or not ? It is convenient for common case to say
    // F(, ...long(g["A"]) , ...)
    // as<T> is the default. Do not use nda objects here, to avoid dependencies.
    // the explicit is crucial here, no implicit conversion
    explicit operator int() const { return as<int>(); }
    explicit operator long() const { return as<long>(); }
    explicit operator double() const { return as<double>(); }
    explicit operator dcomplex() const { return as<dcomplex>(); }

    // ==============  Iterator =============================
    // iterating on *this yields proxy for all elements, dataset and groups
    // impl note Need to keep the range or generator alive after begin call
    // seems hard with ranges, the type is much simpler with a simple coroutine.
    //
    private:
    generator<proxy> get_children() const {
      for (auto key : group(*this).get_all_subgroup_dataset_names()) co_yield (*this)[key];
    }
    mutable std::shared_ptr<generator<proxy>> children; // generator can not be copy-constructed
    // can make it unique_ptr and then redo the copy constructor ...

    public:
    // If a group, we iterate over all elements, dataset and subgroups
    [[nodiscard]] auto begin() const {
      assert_is_group();
      if (not children) children = std::make_shared<generator<proxy>>(get_children());
      return children->begin();
    }
    [[nodiscard]] auto end() const {
      assert(children);
      return children->end();
    }

    // ============== Other iterations =============================

    private:
    [[nodiscard]] auto _read_transform() const {
      // need to capture a copy cf sort_keys later
      return std::views::transform([self = *this](auto &&key) { return self[key]; });
    }

    public:
    // FIXME should we make them free function
    // for (auto gg : groups(g["a"]))
    // for (auto gg : g["a"].groups())) ??
    // If a group, iterate on subgroups only
    [[nodiscard]] auto groups() const { return group(*this).get_all_subgroup_names() | _read_transform(); }

    // If a group, iterate on datasets only
    [[nodiscard]] auto datasets() const { return group(*this).get_all_dataset_names() | _read_transform(); }

    // Returns the keys if a group
    [[nodiscard]] auto keys() const { return group(*this).get_all_subgroup_dataset_names(); }

    // for (auto gg : sort_keys(g["a"], p))
    // iterates on the keys sorted by the predicate p
    // Should we name it sorted_keys ?
    friend auto sort_keys(proxy const &x, auto pred) {
      auto y = group(x).get_all_subgroup_dataset_names();
      std::ranges::sort(y, pred);
      return std::move(y) | x._read_transform();
    }
  };

  // ---------------------------------------------------
  inline auto sort_keys_as_int(proxy const &x) {
    return sort_keys(x, [](auto &&a, auto &&b) { return std::stoi(a) < std::stoi(b); });
  }

} // namespace h5
