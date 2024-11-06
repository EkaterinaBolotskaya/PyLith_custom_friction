# PyLith_custom_friction
Custom friction (failure) laws implemented for [PyLith v2](https://github.com/geodynamics/pylith/tree/releases/v2.2) used in:

E. Bolotskaya, B.H. Hager, and J-P. Ampuero; Effects of failure parameterization on pre- and co-seismic earthquake rupture (submitted to GJI)

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

where $\mu$ is the friction coefficient, $\mu_s$ and $\mu_d$ are the static and dynamic friction coefficients, $D$ is the block slip, and $D_1$ and $D_2$ are the slip-strengthening (or "slip-shift") and slip-weakening (or "slip-stretch") distances respectively.

## Repository contents
- friction/ directory
- spatialdb_examples/ directory
- README.md
- LICENSE

### *friction/*
Contains all the files needed to implement custom friction laws in PyLith v2. The directory contains the original ViscousFriction example files and the 3 new friction model files:
- `DoubleSlipWeakeningFrictionNoHeal`*
- `ExponentialCohesiveZoneNoHeal`*
- `ParabolicCohesiveZoneNoHeal`*

\* Healing is not implemented in these laws. If you're interested in implementing it, refer to the `SlipWeakening` friction in original PyLith and the `forceHealing` flag for guidance.

To use: substitute the contents of [pylith/templates/friction](https://github.com/geodynamics/pylith/tree/releases/v2.2/templates/friction) with the contents of friction/ directory here and build/install as described in [friction/README](https://github.com/geodynamics/pylith/blob/releases/v2.2/templates/friction/README)

Here are the energy curves (shear stress vs. slip) recovered from PyLith dynamic rupture simulations with these custom friction laws (color - node position along the fault).
Slip-weakening friction (SW) is also shown.

<p align="center">
  <img src="https://github.com/user-attachments/assets/912c4c97-e593-4d16-8a0c-fcea70e5225d" width="600"/>
</p>

### *spatialdb_examples/*
Contains 3 example spatialdb files for the new friction laws.

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

## Reference
The results obtained using these failure laws are part of my Thesis: Bolotskaya, E., 2023. Effects of fault failure parameterization and bulk rheology on earthquake rupture (Doctoral dissertation, Massachusetts Institute of Technology)

and a publication submitted to GJI: E. Bolotskaya, B.H. Hager, and J-P. Ampuero, "Effects of failure parameterization on pre- and co-seismic earthquake rupture"

Please contact me if you use EQcycle_failure_laws for your research and would like a citation before the publication is out.

Release on Zenodo:

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.14047393.svg)](https://doi.org/10.5281/zenodo.14047393)

