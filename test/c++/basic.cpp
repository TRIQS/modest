#include <triqs/test_tools/gfs.hpp>
#include <triqs_modest/triqs_modest.hpp>

using namespace triqs_modest;

TEST(Toto, Add) { // NOLINT

  toto a(0);
  toto b(2);

  auto c = a + b;
  EXPECT_EQ(c, b); // NOLINT
}

TEST(Toto, H5) { // NOLINT

  toto a(0);
  { // Local scope for file
    h5::file f("f.h5", 'w');
    h5_write(f, "a", a);
  }

  toto a2;
  {
    h5::file f("f.h5", 'a');
    h5_read(f, "a", a2);
  }

  EXPECT_EQ(a, a2); // NOLINT
}
