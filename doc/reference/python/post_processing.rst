.. _reference/python/post_processing:

Post-processing
***************

Real-frequency spectral functions, projected spectral functions, and
momentum-resolved spectral functions along high-symmetry paths.

Spectral functions
==================

The **projected spectral function** on the correlated subspace
:math:`\mathcal{C}` is the imaginary part of the real-frequency local
Green's function,

.. math::

   A_{\mathcal{C}}^{\sigma}(\omega)
   = -\frac{1}{\pi}\, \mathrm{Im}\,
     \mathrm{Tr}_{\mathcal{C}}\,
     G_{\mathcal{C},\,\mathrm{loc}}^{\sigma}(\omega + i 0^{+})
   = -\frac{1}{\pi}\, \mathrm{Im}\,
     \mathrm{Tr}_{\mathcal{C}} \sum_{\mathbf{k}}
     P(\mathbf{k})\,
     G_{\mathcal{B}}^{\sigma}(\mathbf{k}, \omega + i 0^{+})\,
     P^{\dagger}(\mathbf{k}),

and is reported either as a scalar trace or resolved by orbital
:math:`m \in \mathcal{C}` (diagonal entries
:math:`A_{m}^{\sigma}(\omega)`).

The **momentum-resolved spectral function along a high-symmetry path**
:math:`\mathbf{k}(s)` is the band-trace of the lattice Green's function
on the chosen path,

.. math::

   A^{\sigma}(\mathbf{k}(s), \omega)
   = -\frac{1}{\pi}\, \mathrm{Im}\, \mathrm{Tr}_{\nu}\,
     G_{\mathcal{B}}^{\sigma}(\mathbf{k}(s), \omega + i 0^{+}),

where :math:`G_{\mathcal{B}}^{\sigma}` is evaluated on the
high-symmetry path with the analytically-continued self-energy.

.. autosummary::

   triqs_modest.post_processing.projected_spectral_function
   triqs_modest.post_processing.spectral_function_on_high_symmetry_path

Container types
===============

.. autosummary::

   triqs_modest.post_processing.SpectralFunctionW
   triqs_modest.post_processing.SpectralFunctionKw
