// -*- C++ -*-
//
// ----------------------------------------------------------------------
//
// Brad T. Aagaard, U.S. Geological Survey
// Charles A. Williams, GNS Science
// Matthew G. Knepley, University of Chicago
//
// This code was developed as part of the Computational Infrastructure
// for Geodynamics (http://geodynamics.org).
//
// Copyright (c) 2010-2017 University of California, Davis
//
// See COPYING for license information.
//
// ----------------------------------------------------------------------
//

/* @brief C++ ParabolicCohesiveZoneNoHeal object that implements friction
 * correlated with slip rate.
 *
 * implemented by Ekaterina Bolotskaya (modified from ViscousFriction example)
 *
 * Friction model in which the friction coefficient evolves parabolically with slip.
 * The physical properties are specified using cohesion, static friciton coefficient (mu_s),
 * dynamic friction coefficient (mu_d), slip_shift or slip-strengthening distance (D_s) 
 * and slip_stretch or slip-weakening distance (D_w).
 *
 *\mu =
    \begin{cases}
      \mu_s - (\mu_s-\mu_d) \left( \frac{D-D_s}{D_w} \right) ^2       & D \le D_s+D_w\\
      \mu_d                                                           & D > D_s+D_w\\
    \end{cases}
 */

#if !defined(pylith_friction_ParabolicCohesiveZoneNoHeal_hh)
#define pylith_friction_ParabolicCohesiveZoneNoHeal_hh

// Include directives ---------------------------------------------------
#include "pylith/friction/FrictionModel.hh" // ISA FrictionModel

// Forward declarations
namespace contrib {
  namespace friction {
    class ParabolicCohesiveZoneNoHeal;
  } // friction
} // pylith

// ParabolicCohesiveZoneNoHeal -------------------------------------------------------
class contrib::friction::ParabolicCohesiveZoneNoHeal : public pylith::friction::FrictionModel
{ // class ParabolicCohesiveZoneNoHeal
  friend class TestParabolicCohesiveZoneNoHeal; // unit testing

  // --------------------------------------------------------------------
  // All of these functions are required to satisfy the PyLith
  // interface for a fault constitutive model.
  // --------------------------------------------------------------------

  // PUBLIC METHODS /////////////////////////////////////////////////////
public :

  /// Default constructor.
  ParabolicCohesiveZoneNoHeal(void);

  /// Destructor.
  ~ParabolicCohesiveZoneNoHeal(void);

  // PROTECTED METHODS //////////////////////////////////////////////////
protected :

  /// These methods should be implemented by every constitutive model.

  /** Compute properties from values in spatial database.
   *
   * @param propValues Array of property values.
   * @param dbValues Array of database values.
   */
  void _dbToProperties(PylithScalar* const propValues,
		       const pylith::scalar_array& dbValues) const;

  /** Nondimensionalize properties.
   *
   * @param values Array of property values.
   * @param nvalues Number of values.
   */
  void _nondimProperties(PylithScalar* const values,
			 const int nvalues) const;

  /** Dimensionalize properties.
   *
   * @param values Array of property values.
   * @param nvalues Number of values.
   */
  void _dimProperties(PylithScalar* const values,
		      const int nvalues) const;

  /** Compute friction from properties and state variables.
   *
   * @param slip Current slip at location.
   * @param slipRate Current slip rate at location.
   * @param normalTraction Normal traction at location.
   * @param properties Properties at location.
   * @param numProperties Number of properties.
   * @param stateVars State variables at location.
   * @param numStateVars Number of state variables.
   */
  void _dbToStateVars(PylithScalar* const stateValues,
		      const pylith::scalar_array& dbValues) const;

  /** Nondimensionalize state variables.
   *
   * @param values Array of initial state values.
   * @param nvalues Number of values.
   */
  void _nondimStateVars(PylithScalar* const values,
			const int nvalues) const;
  
  /** Dimensionalize state variables.
   *
   * @param values Array of initial state values.
   * @param nvalues Number of values.
   */
  void _dimStateVars(PylithScalar* const values,
		     const int nvalues) const;

  /** Compute friction from properties and state variables.
   *
   * @param t Time in simulation.
   * @param slip Current slip at location.
   * @param slipRate Current slip rate at location.
   * @param normalTraction Normal traction at location.
   * @param properties Properties at location.
   * @param numProperties Number of properties.
   * @param stateVars State variables at location.
   * @param numStateVars Number of state variables.
   */
  PylithScalar _calcFriction(const PylithScalar t,
			     const PylithScalar slip,
			     const PylithScalar slipRate,
			     const PylithScalar normalTraction,
			     const PylithScalar* properties,
			     const int numProperties,
			     const PylithScalar* stateVars,
			     const int numStateVars);

  /** Compute derivative friction with slip from properties and state
   * variables.
   *
   * @param t Time in simulation.
   * @param slip Current slip at location.
   * @param slipRate Current slip rate at location.
   * @param normalTraction Normal traction at location.
   * @param properties Properties at location.
   * @param numProperties Number of properties.
   * @param stateVars State variables at location.
   * @param numStateVars Number of state variables.
   *
   * @returns Derivative of friction (magnitude of shear traction) at vertex.
   */
  PylithScalar _calcFrictionDeriv(const PylithScalar t,
				  const PylithScalar slip,
				  const PylithScalar slipRate,
				  const PylithScalar normalTraction,
				  const PylithScalar* properties,
				  const int numProperties,
				  const PylithScalar* stateVars,
				  const int numStateVars);
  
  // --------------------------------------------------------------------
  // Optional function in the PyLith interface for a fault
  // constitutive model. Even though this function is optional, for it
  // to be used it the interface must exactly matched the one
  // specified in FrictionModel.
  // --------------------------------------------------------------------

  /** Update state variables (for next time step).
   *
   * @param t Time in simulation.
   * @param slip Current slip at location.
   * @param slipRate Current slip rate at location.
   * @param normalTraction Normal traction at location.
   * @param stateVars State variables at location.
   * @param numStateVars Number of state variables.
   * @param properties Properties at location.
   * @param numProperties Number of properties.
   */
  void _updateStateVars(const PylithScalar t,
			const PylithScalar slip,
			const PylithScalar slipRate,
			const PylithScalar normalTraction,
			PylithScalar* const stateVars,
			const int numStateVars,
			const PylithScalar* properties,
			const int numProperties);

  // PRIVATE MEMBERS ////////////////////////////////////////////////////
private :

  // --------------------------------------------------------------------
  // We use these constants for consistent access into the arrays of
  // physical properties and state variables.
  // --------------------------------------------------------------------

  static const int p_coefS;
  static const int p_coefD;
  static const int p_slShift;
  static const int p_slStretch;
  static const int p_cohesion;
  static const int db_coefS;
  static const int db_coefD;
  static const int db_slShift;
  static const int db_slStretch;
  static const int db_cohesion;

  /// Indices for state variables in section and spatial database.
  static const int s_slipCum;
  static const int s_slipPrev;

  static const int db_slipCum;
  static const int db_slipPrev;

  // NOT IMPLEMENTED ////////////////////////////////////////////////////
private :

  ParabolicCohesiveZoneNoHeal(const ParabolicCohesiveZoneNoHeal&); ///< Not implemented.
  const ParabolicCohesiveZoneNoHeal& operator=(const ParabolicCohesiveZoneNoHeal&); ///< Not implemented

}; // class ParabolicCohesiveZoneNoHeal

#endif // pylith_friction_ParabolicCohesiveZoneNoHeal_hh


// End of file 
