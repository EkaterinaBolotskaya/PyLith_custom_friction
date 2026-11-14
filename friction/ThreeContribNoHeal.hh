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

/* @brief C++ ThreeContribNoHeal object that implements friction
 * correlated with slip.
 *
 * implemented by Ekaterina Bolotskaya (modified from ViscousFriction example)
 *
 * Friction model in which the friction coefficient is the sum of three
 * contributions of cumulative slip D (no healing): dilation, Coulomb
 * friction and damage. The total is bounded below by the steady-state
 * friction coefficient mu_ss.
 *
 * \mu = \max(\mu_{dil} + \mu_{fric} + \mu_{dam}, \mu_{ss})
 *
 * \mu_{dil} = \tan(\psi_p) \exp\left(-\ln 2 \left(\frac{D-u_{dp}}{u_{dhalf}-u_{dp}}\right)^2\right)
 *
 * \mu_{fric} =
 *  \begin{cases}
 *     k D                                                          &  D < u_{fp}\\
 *     \mu_{fp} + (\mu_{fpp}-\mu_{fp})\frac{D-u_{fp}}{\Delta u_{fpp}} &  u_{fp} \le D < u_{fpp}\\
 *     \mu_{ss} + (\mu_{fpp}-\mu_{ss})\exp\left(-\frac{D-u_{fpp}}{\Delta u_f}\right) &  D \ge u_{fpp}\\
 *  \end{cases}
 *  with u_{fp} = \mu_{fp}/k and u_{fpp} = u_{fp} + \Delta u_{fpp}
 *  (u_{fpp} = u_{fp} if \mu_{fp} \le \mu_{ss})
 *
 * \mu_{dam} =
 *  \begin{cases}
 *     0                                                &  D < u_{dam}\\
 *     \mu_{dp}\exp\left(-\frac{D-u_{dam}}{\Delta u_f}\right) &  D \ge u_{dam}\\
 *  \end{cases}
 *
 * Properties: psi_p (degrees), u_dp, u_dhalf, k (1/length), mu_fpp, mu_fp,
 * delta_u_fpp, mu_ss, delta_u_f, u_dam, mu_dp.
 * State variables: cumulative_slip, previous_slip.
 */

#if !defined(pylith_friction_ThreeContribNoHeal_hh)
#define pylith_friction_ThreeContribNoHeal_hh

// Include directives ---------------------------------------------------
#include "pylith/friction/FrictionModel.hh" // ISA FrictionModel

// Forward declarations
namespace contrib {
  namespace friction {
    class ThreeContribNoHeal;
  } // friction
} // pylith

// ThreeContribNoHeal -------------------------------------------------------
class contrib::friction::ThreeContribNoHeal : public pylith::friction::FrictionModel
{ // class ThreeContribNoHeal
  friend class TestThreeContribNoHeal; // unit testing

  // --------------------------------------------------------------------
  // All of these functions are required to satisfy the PyLith
  // interface for a fault constitutive model.
  // --------------------------------------------------------------------

  // PUBLIC METHODS /////////////////////////////////////////////////////
public :

  /// Default constructor.
  ThreeContribNoHeal(void);

  /// Destructor.
  ~ThreeContribNoHeal(void);

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

  static const int p_psi_p;
  static const int p_u_dp;
  static const int p_u_dhalf;
  static const int p_k;
  static const int p_mu_fpp;
  static const int p_mu_fp;
  static const int p_delta_u_fpp;
  static const int p_mu_ss;
  static const int p_delta_u_f;
  static const int p_u_dam;
  static const int p_mu_dp;
  static const int db_psi_p;
  static const int db_u_dp;
  static const int db_u_dhalf;
  static const int db_k;
  static const int db_mu_fpp;
  static const int db_mu_fp;
  static const int db_delta_u_fpp;
  static const int db_mu_ss;
  static const int db_delta_u_f;
  static const int db_u_dam;
  static const int db_mu_dp;

  /// Indices for state variables in section and spatial database.
  static const int s_slipCum;
  static const int s_slipPrev;

  static const int db_slipCum;
  static const int db_slipPrev;

  // NOT IMPLEMENTED ////////////////////////////////////////////////////
private :

  ThreeContribNoHeal(const ThreeContribNoHeal&); ///< Not implemented.
  const ThreeContribNoHeal& operator=(const ThreeContribNoHeal&); ///< Not implemented

}; // class ThreeContribNoHeal

#endif // pylith_friction_ThreeContribNoHeal_hh


// End of file 
