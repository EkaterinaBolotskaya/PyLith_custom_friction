# PyLith_custom_friction
Custom friction (failure) laws implemented for [PyLith v2](https://github.com/geodynamics/pylith/tree/releases/v2.2) used in:

- Double slip-weakening (DSW), exponential cohesive zone (ECZ), and parabolic cohesive zone (PCZ) models: Bolotskaya, E., Hager, B., & Ampuero, J. P. (2025). Effects of failure parameterization on pre- and co-seismic earthquake rupture. Geophysical Journal International, 240(3), 1538-1550.
- Three-contribution (3C) model: Bolotskaya, E., Casas, N., & Mollon, G. (in preparation).

## Friction laws

<ins>__Double slip-weakening (DSW) model:__</ins>

$$ \mu =
   \begin{cases}
      \mu_s - (\mu_s-\mu_t)\frac{D}{D_{w1}}                   &  D < D_{w1}\\
      \mu_t - (\mu_t-\mu_d)\frac{D-D_{w1}}{D_{w2}-D_{w1}}     &  D_{w1} \le D \le D_{w2}\\
      \mu_d                                                   &  D > D_{w2}\\
   \end{cases}
    , \qquad \qquad \qquad \qquad (1)
$$

where $\mu_s$, $\mu_t$, and $\mu_d$ are the static, transitional, and dynamic friction coefficients, $D_{w1}$ is the transition slip distance and $D_{w2}$ is the final slip distannce. The last segment of the failure law is horizontal with $\mu=\mu_d$.


<ins>__Exponential cohesive zone (ECZ):__</ins>

$$
\mu = \mu_d + (\mu_s-\mu_d)\frac{D+D_1}{D_2} e^{1-\frac{D+D_1}{D_2}} ,  \qquad \qquad \qquad \qquad \qquad \qquad (2)
$$

where $\mu$ is the friction coefficient, $\mu_s$ and $\mu_d$ are the static and dynamic friction coefficients, and $D_1$ and $D_2$ are the two slip distances which we call "slip-shift" and "slip-stretch" respectively.

<ins>__Parabolic cohesive zone (PCZ):__</ins>

$$
\mu =
    \begin{cases}
      \mu_s - (\mu_s-\mu_d) \left( \frac{D-D_1}{D_2} \right) ^2              & D \le D_1+D_2\\
      \mu_d                                            & D > D_1+D_2\\
    \end{cases} 
    ,  \qquad \qquad \qquad \qquad (3)
$$

where $\mu$ is the friction coefficient, $\mu_s$ and $\mu_d$ are the static and dynamic friction coefficients, $D_1$ and $D_2$ are the slip-strengthening (or "slip-shift") and slip-weakening (or "slip-stretch") distances respectively.

<ins>__Three-contribution model (3C):__</ins>

$$
\mu = \max\left(\mu_{dil} + \mu_{fric} + \mu_{dam},\ \mu_{ss}\right) ,  \qquad \qquad \qquad \qquad \qquad \qquad (4)
$$

$$
\mu_{dil} = \tan\psi_p \ e^{-\ln 2 \left(\frac{D-u_{dp}}{u_{dhalf}-u_{dp}}\right)^2}
$$

$$
\mu_{fric} =
   \begin{cases}
      k D                                                                     &  D < u_{fp}\\
      \mu_{fp} + (\mu_{fpp}-\mu_{fp})\frac{D-u_{fp}}{\Delta u_{fpp}}          &  u_{fp} \le D < u_{fpp}\\
      \mu_{ss} + (\mu_{fpp}-\mu_{ss}) e^{-\frac{D-u_{fpp}}{\Delta u_f}}       &  D \ge u_{fpp}\\
   \end{cases}
$$

$$
\mu_{dam} =
   \begin{cases}
      0                                                 &  D < u_{dam}\\
      \mu_{dp} e^{-\frac{D-u_{dam}}{\Delta u_f}}        &  D \ge u_{dam}\\
   \end{cases}
$$

where $u_{fp} = \mu_{fp}/k$ and $u_{fpp} = u_{fp} + \Delta u_{fpp}$ (if $\mu_{fp} \le \mu_{ss}$, the linear weakening segment is skipped and $u_{fpp} = u_{fp}$). The friction coefficient is the sum of a dilation term with peak dilation angle $\psi_p$ (in degrees) reached at slip $u_{dp}$ and halved at slip $u_{dhalf}$; a Coulomb term that rises linearly with slope $k$ to the peak $\mu_{fp}$, drops linearly to $\mu_{fpp}$ over $\Delta u_{fpp}$, and then decays exponentially to the steady-state value $\mu_{ss}$ over $\Delta u_f$; and a damage term with peak $\mu_{dp}$ starting at slip $u_{dam}$ and decaying over $\Delta u_f$. The total is bounded below by $\mu_{ss}$.

## Repository contents
- friction/ directory
- spatialdb_examples/ directory
- README.md
- LICENSE

### *friction/*
Contains all the files needed to implement custom friction laws in PyLith v2. The directory contains the original ViscousFriction example files and the 4 new friction model files:
- `DoubleSlipWeakeningFrictionNoHeal`*
- `ExponentialCohesiveZoneNoHeal`*
- `ParabolicCohesiveZoneNoHeal`*
- `ThreeContribNoHeal`*

\* Healing is not implemented in these laws. If you're interested in implementing it, refer to the `SlipWeakening` friction in original PyLith and the `forceHealing` flag for guidance.

To use: substitute the contents of [pylith/templates/friction](https://github.com/geodynamics/pylith/tree/releases/v2.2/templates/friction) with the contents of friction/ directory here and build/install as described in [friction/README](https://github.com/geodynamics/pylith/blob/releases/v2.2/templates/friction/README)

Here are the energy curves (shear stress vs. slip) recovered from PyLith dynamic rupture simulations with the DSW, ECZ, and PCZ friction laws (color - node position along the fault).
Slip-weakening friction (SW) is also shown.

<p align="center">
  <img src="https://github.com/user-attachments/assets/912c4c97-e593-4d16-8a0c-fcea70e5225d" width="600"/>
</p>

### *spatialdb_examples/*
Contains 4 example spatialdb files for the new friction laws.

Use in .cfg file (DSW):
```
[pylithapp.timedependent.interfaces.fault]
# Use DSW friction.
friction = pylith.friction.contrib.DoubleSlipWeakeningFrictionNoHeal
friction.label = DoubleSlipWeakeningFrictionNoHeal

# Set the friction model parameters.
friction.db_properties = spatialdata.spatialdb.SimpleDB
friction.db_properties.label = DoubleSlipWeakeningFrictionNoHeal_properties
friction.db_properties.iohandler.filename = spatialdb/1_friction_DSW.spatialdb.spatialdb
```

Use in .cfg file (ECZ):
```
[pylithapp.timedependent.interfaces.fault]
# Use ECZ friction.
friction = pylith.friction.contrib.ExponentialCohesiveZoneNoHeal
friction.label = ExponentialCohesiveZoneNoHeal

# Set the friction model parameters.
friction.db_properties = spatialdata.spatialdb.SimpleDB
friction.db_properties.label = ExponentialCohesiveZoneNoHeal_properties
friction.db_properties.iohandler.filename = spatialdb/2_friction_EXP.spatialdb
```

Use in .cfg file (PCZ):
```
[pylithapp.timedependent.interfaces.fault]
# Use PCZ friction.
friction = pylith.friction.contrib.ParabolicCohesiveZoneNoHeal
friction.label = ParabolicCohesiveZoneNoHeal

# Set the friction model parameters.
friction.db_properties = spatialdata.spatialdb.SimpleDB
friction.db_properties.label = ParabolicCohesiveZoneNoHeal_properties
friction.db_properties.iohandler.filename = spatialdb/3_friction_PAR.spatialdb
```

Use in .cfg file (3C):
```
[pylithapp.timedependent.interfaces.fault]
# Use 3C friction.
friction = pylith.friction.contrib.ThreeContribNoHeal
friction.label = ThreeContribNoHeal

# Set the friction model parameters.
friction.db_properties = spatialdata.spatialdb.SimpleDB
friction.db_properties.label = ThreeContribNoHeal_properties
friction.db_properties.iohandler.filename = spatialdb/4_friction_3C.spatialdb
```

The state variables of the no-healing laws are `cumulative_slip` and `previous_slip`. To start a simulation from non-zero slip (e.g. a dynamic run initialized from a quasi-static run), set them with `friction.db_initial_state`, with `previous_slip = 0`: the simulation starts from zero slip, so a non-zero `previous_slip` is added to `cumulative_slip` again on the first time step.

## Reference
Please refer the following article if you use the DSW, ECZ, or PCZ models from PyLith_custom_friction for your research:

Bolotskaya, E., Hager, B., & Ampuero, J. P. (2025). Effects of failure parameterization on pre-and co-seismic earthquake rupture. Geophysical Journal International, 240(3), 1538-1550.

or

[my Thesis](https://scholar.google.com/citations?view_op=view_citation&hl=en&user=z2lDhmcAAAAJ&citation_for_view=z2lDhmcAAAAJ:YsMSGLbcyi4C): Bolotskaya, E., 2023. Effects of fault failure parameterization and bulk rheology on earthquake rupture (Doctoral dissertation, Massachusetts Institute of Technology)

Please refer the following article if you use the 3C model (`ThreeContribNoHeal`) for your research:

Bolotskaya, E., Casas, N., & Mollon, G. (in preparation).

Release on Zenodo:

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.14047393.svg)](https://doi.org/10.5281/zenodo.14047393)

