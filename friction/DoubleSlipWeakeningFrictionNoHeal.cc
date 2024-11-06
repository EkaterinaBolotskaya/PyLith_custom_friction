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
// implemented by Ekaterina Bolotskaya (modified from ViscousFriction example)

// See DoubleSlipWeakeningFrictionNoHeal.hh for a description of each C++ function and
// its arguments.

#include <portinfo> // machine specific info generated by configure

#include "DoubleSlipWeakeningFrictionNoHeal.hh" // implementation of object methods

#include "pylith/materials/Metadata.hh" // USES Metadata

#include "pylith/utils/array.hh" // USES scaary_array
#include "pylith/utils/constdefs.h" // USES PYLITH_MAXSCALAR

#include "spatialdata/units/Nondimensional.hh" // USES Nondimensional

#include <cassert> // USES assert()
#include <sstream> // USES std::ostringstream
#include <stdexcept> // USES std::runtime_error

// ----------------------------------------------------------------------
// Create a local namespace to use for local constants and other
// information. This insulates all other classes from this information
// while preventing clashes with other local constants and data (as
// long as no other object use the same _DoubleSlipWeakeningFrictionNoHeal namespace in
// the  namespace.
namespace contrib {
  namespace friction {
    namespace _DoubleSlipWeakeningFrictionNoHeal {

      // These are the fault constitutive parameters stored during the
      // simulation and need not coincide with the physical properties
      // provided by the user.

      // Number of physical properties.
      const int numProperties = 6;

      // Friction model parameters.
      const pylith::materials::Metadata::ParamDescription properties[numProperties] = {
	{ "static_coefficient", 1, pylith::topology::FieldBase::SCALAR },
	{ "transition_coefficient", 1, pylith::topology::FieldBase::SCALAR },
	{ "dynamic_coefficient", 1, pylith::topology::FieldBase::SCALAR },
	{ "transition_slip_distance", 1, pylith::topology::FieldBase::SCALAR },
	{ "final_slip_distance", 1, pylith::topology::FieldBase::SCALAR },
	{ "cohesion", 1, pylith::topology::FieldBase::SCALAR },
      };

      // Number of state variables.
      const int numStateVars = 2;

      // State Variables.
      const pylith::materials::Metadata::ParamDescription stateVars[] = {
	{ "cumulative_slip", 1, pylith::topology::FieldBase::SCALAR },
	{ "previous_slip", 1, pylith::topology::FieldBase::SCALAR },
      };

      // Values expected in spatial database
      const int numDBProperties = 6;
      const char* dbProperties[numDBProperties] = { 
	"static_coefficient",
	"transition_coefficient",
	"dynamic_coefficient",
	"transition_slip_distance",
	"final_slip_distance",
	"cohesion",
      };

      // These are the state variables stored during the simulation. 

      const int numDBStateVars = 2;
      const char* dbStateVars[numDBStateVars] = { "cumulative_slip",
	"previous_slip",
      };      
      
    } // _DoubleSlipWeakeningFrictionNoHeal
  } // friction
} // contrib

// Indices of fault constitutive parameters.
const int contrib::friction::DoubleSlipWeakeningFrictionNoHeal::p_coefS = 0;
const int contrib::friction::DoubleSlipWeakeningFrictionNoHeal::p_coefT = 
  contrib::friction::DoubleSlipWeakeningFrictionNoHeal::p_coefS + 1;
const int contrib::friction::DoubleSlipWeakeningFrictionNoHeal::p_coefD = 
  contrib::friction::DoubleSlipWeakeningFrictionNoHeal::p_coefT + 1;
const int contrib::friction::DoubleSlipWeakeningFrictionNoHeal::p_distT = 
  contrib::friction::DoubleSlipWeakeningFrictionNoHeal::p_coefD + 1;
const int contrib::friction::DoubleSlipWeakeningFrictionNoHeal::p_distF = 
  contrib::friction::DoubleSlipWeakeningFrictionNoHeal::p_distT + 1;
const int contrib::friction::DoubleSlipWeakeningFrictionNoHeal::p_cohesion =
  contrib::friction::DoubleSlipWeakeningFrictionNoHeal::p_distF + 1;

// Indices of database values (order must match dbProperties)
const int contrib::friction::DoubleSlipWeakeningFrictionNoHeal::db_coefS = 0;
const int contrib::friction::DoubleSlipWeakeningFrictionNoHeal::db_coefT =
  contrib::friction::DoubleSlipWeakeningFrictionNoHeal::db_coefS + 1;
const int contrib::friction::DoubleSlipWeakeningFrictionNoHeal::db_coefD =
  contrib::friction::DoubleSlipWeakeningFrictionNoHeal::db_coefT + 1;
const int contrib::friction::DoubleSlipWeakeningFrictionNoHeal::db_distT =
  contrib::friction::DoubleSlipWeakeningFrictionNoHeal::db_coefD + 1;
const int contrib::friction::DoubleSlipWeakeningFrictionNoHeal::db_distF =
  contrib::friction::DoubleSlipWeakeningFrictionNoHeal::db_distT + 1;
const int contrib::friction::DoubleSlipWeakeningFrictionNoHeal::db_cohesion =
  contrib::friction::DoubleSlipWeakeningFrictionNoHeal::db_distF + 1;

// Indices of state variables.
const int contrib::friction::DoubleSlipWeakeningFrictionNoHeal::s_slipCum = 0;
const int contrib::friction::DoubleSlipWeakeningFrictionNoHeal::s_slipPrev = 
  contrib::friction::DoubleSlipWeakeningFrictionNoHeal::s_slipCum + 1;

// Indices of database values (order must match dbProperties)
const int contrib::friction::DoubleSlipWeakeningFrictionNoHeal::db_slipCum = 0;
const int contrib::friction::DoubleSlipWeakeningFrictionNoHeal::db_slipPrev = 
  contrib::friction::DoubleSlipWeakeningFrictionNoHeal::db_slipCum + 1;

// ----------------------------------------------------------------------
// Default constructor.
contrib::friction::DoubleSlipWeakeningFrictionNoHeal::DoubleSlipWeakeningFrictionNoHeal(void) :
  pylith::friction::FrictionModel(pylith::materials::Metadata(_DoubleSlipWeakeningFrictionNoHeal::properties,
				    _DoubleSlipWeakeningFrictionNoHeal::numProperties,
				    _DoubleSlipWeakeningFrictionNoHeal::dbProperties,
				    _DoubleSlipWeakeningFrictionNoHeal::numDBProperties,
				    _DoubleSlipWeakeningFrictionNoHeal::stateVars,
				    _DoubleSlipWeakeningFrictionNoHeal::numStateVars,
				    _DoubleSlipWeakeningFrictionNoHeal::dbStateVars,
				    _DoubleSlipWeakeningFrictionNoHeal::numDBStateVars))
                    
{ // constructor
} // constructor

// ----------------------------------------------------------------------
// Destructor.
contrib::friction::DoubleSlipWeakeningFrictionNoHeal::~DoubleSlipWeakeningFrictionNoHeal(void)
{ // destructor
} // destructor

// ----------------------------------------------------------------------
// Compute properties from values in spatial database.
void
contrib::friction::DoubleSlipWeakeningFrictionNoHeal::_dbToProperties(PylithScalar* const propValues,
						    const pylith::scalar_array& dbValues) const
{ // _dbToProperties
  // Check consistency of arguments
  assert(propValues);
  const int numDBValues = dbValues.size();
  assert(_DoubleSlipWeakeningFrictionNoHeal::numDBProperties == numDBValues);

  // Extract values from array using our defined indices.
  const PylithScalar coefS = dbValues[db_coefS];
  const PylithScalar coefT = dbValues[db_coefT];
  const PylithScalar coefD = dbValues[db_coefD];
  const PylithScalar distT = dbValues[db_distT];
  const PylithScalar distF = dbValues[db_distF];
  const PylithScalar cohesion = dbValues[db_cohesion];

  // Check for reasonable values. If user supplied unreasonable values
  // throw an exception.
  if (coefS <= 0.0) {
    std::ostringstream msg;
    msg << "Spatial database returned nonpositive value for static "
	<< "coefficient of friction.\n"
	<< "Static coefficient of friction: " << coefS << "\n";
    throw std::runtime_error(msg.str());
  } // if
  
  if (coefT <= 0.0) {
    std::ostringstream msg;
    msg << "Spatial database returned nonpositive value for transition "
	<< "coefficient of friction.\n"
	<< "Transition coefficient of friction: " << coefT << "\n";
    throw std::runtime_error(msg.str());
  } // if
  
  if (coefD <= 0.0) {
    std::ostringstream msg;
    msg << "Spatial database returned nonpositive value for dynamic "
	<< "coefficient of friction.\n"
	<< "Dynamic coefficient of friction: " << coefD << "\n";
    throw std::runtime_error(msg.str());
  } // if
  
  if (distT <= 0.0) {
    std::ostringstream msg;
    msg << "Spatial database returned nonpositive value for transition "
	<< "slip weakening distance.\n"
	<< "Transition slip weakening distance: " << distT << "\n";
    throw std::runtime_error(msg.str());
  } // if
  
  if (distF <= 0.0) {
    std::ostringstream msg;
    msg << "Spatial database returned nonpositive value for final "
	<< "slip weakening distance.\n"
	<< "Final slip weakening distance: " << distF << "\n";
    throw std::runtime_error(msg.str());
  } // if

  if (cohesion < 0.0) {
    std::ostringstream msg;
    msg << "Spatial database returned negative value for cohesion.\n"
	<< "Cohesion: " << cohesion << "\n";
    throw std::runtime_error(msg.str());
  } // if

  // Compute parameters that we store from the user-supplied parameters.
  propValues[p_coefS] = coefS;
  propValues[p_coefT] = coefT;
  propValues[p_coefD] = coefD;
  propValues[p_distT] = distT;
  propValues[p_distF] = distF;
  propValues[p_cohesion] = cohesion;

} // _dbToProperties

// ----------------------------------------------------------------------
// Nondimensionalize properties.
void
contrib::friction::DoubleSlipWeakeningFrictionNoHeal::_nondimProperties(PylithScalar* const values,
						      const int nvalues) const
{ // _nondimProperties
  // Check consistency of arguments.
  assert(_normalizer);
  assert(values);
  assert(nvalues == _DoubleSlipWeakeningFrictionNoHeal::numProperties);

  // Get scales needed to nondimensional parameters from the
  // Nondimensional object.
  const PylithScalar lengthScale = _normalizer->lengthScale();
  const PylithScalar timeScale = _normalizer->timeScale();
  const PylithScalar pressureScale = _normalizer->pressureScale();
  const PylithScalar velocityScale = lengthScale / timeScale;

  // Use the Nondimensional::nondimensionalize() function to
  // nondimensionalize the quantities using the appropriate scale.
  values[p_distT] = _normalizer->nondimensionalize(values[p_distT], lengthScale);
  values[p_distF] = _normalizer->nondimensionalize(values[p_distF], lengthScale);
  values[p_cohesion] = 
    _normalizer->nondimensionalize(values[p_cohesion], pressureScale);
} // _nondimProperties

// ----------------------------------------------------------------------
// Dimensionalize properties.
void
contrib::friction::DoubleSlipWeakeningFrictionNoHeal::_dimProperties(PylithScalar* const values,
						   const int nvalues) const
{ // _dimProperties
  // Check consistency of arguments.
  assert(_normalizer);
  assert(values);
  assert(nvalues == _DoubleSlipWeakeningFrictionNoHeal::numProperties);

  // Get scales needed to dimensional parameters from the
  // Nondimensional object.
  const PylithScalar lengthScale = _normalizer->lengthScale();
  const PylithScalar timeScale = _normalizer->timeScale();
  const PylithScalar pressureScale = _normalizer->pressureScale();
  const PylithScalar velocityScale = lengthScale / timeScale;

  // Use the Nondimensional::dimensionalize() function to
  // dimensionalize the quantities using the appropriate scale.
  values[p_distT] = _normalizer->dimensionalize(values[p_distT], lengthScale);
  values[p_distF] = _normalizer->dimensionalize(values[p_distF], lengthScale);
  values[p_cohesion] = 
    _normalizer->dimensionalize(values[p_cohesion], pressureScale);
} // _dimProperties

// ----------------------------------------------------------------------
// Compute state variables from values in spatial database.
void
contrib::friction::DoubleSlipWeakeningFrictionNoHeal::_dbToStateVars(PylithScalar* const stateValues,
						   const pylith::scalar_array& dbValues) const
{ // _dbToStateVars
  // Check consistency of arguments.
  assert(stateValues);
  const int numDBValues = dbValues.size();
  assert(_DoubleSlipWeakeningFrictionNoHeal::numDBStateVars == numDBValues);

  // Compute friction parameters that we store from the user-supplied
  // friction parameters.
  const PylithScalar cumulativeSlip = dbValues[db_slipCum];
  const PylithScalar previousSlip = dbValues[db_slipPrev];
 
  // Store computed friction parameters in the properties array.
  stateValues[s_slipCum] = cumulativeSlip;
  stateValues[s_slipPrev] = previousSlip;
} // _dbToStateVars

// ----------------------------------------------------------------------
// Nondimensionalize state variables.
void
contrib::friction::DoubleSlipWeakeningFrictionNoHeal::_nondimStateVars(PylithScalar* const values,
						     const int nvalues) const
{ // _nondimStateVars
  // Check consistency of arguments.
  assert(_normalizer);
  assert(values);
  assert(nvalues == _DoubleSlipWeakeningFrictionNoHeal::numStateVars);

  // Get scales needed to nondimensional parameters from the
  // Nondimensional object.
  const PylithScalar lengthScale = _normalizer->lengthScale();
  const PylithScalar timeScale = _normalizer->timeScale();
  const PylithScalar velocityScale = lengthScale / timeScale;

  // Use the Nondimensional::dimensionalize() function to
  // dimensionalize the quantities using the appropriate scale.
  values[s_slipCum] = _normalizer->nondimensionalize(values[s_slipCum], lengthScale);
  values[s_slipPrev] = _normalizer->nondimensionalize(values[s_slipPrev], lengthScale);
} // _nondimStateVars

// ----------------------------------------------------------------------
// Dimensionalize state variables.
void
contrib::friction::DoubleSlipWeakeningFrictionNoHeal::_dimStateVars(PylithScalar* const values,
						  const int nvalues) const
{ // _dimStateVars
  // Check consistency of arguments.
  assert(_normalizer);
  assert(values);
  assert(nvalues == _DoubleSlipWeakeningFrictionNoHeal::numStateVars);

  // Get scales needed to dimensional parameters from the
  // Nondimensional object.
  const PylithScalar lengthScale = _normalizer->lengthScale();
  const PylithScalar timeScale = _normalizer->timeScale();
  const PylithScalar velocityScale = lengthScale / timeScale;

  // Use the Nondimensional::dimensionalize() function to
  // dimensionalize the quantities using the appropriate scale.
  values[s_slipCum] = _normalizer->dimensionalize(values[s_slipCum], lengthScale);
  values[s_slipPrev] = _normalizer->dimensionalize(values[s_slipPrev], lengthScale);
} // _dimStateVars

// ----------------------------------------------------------------------
// Compute friction from properties and state variables.
PylithScalar
contrib::friction::DoubleSlipWeakeningFrictionNoHeal::_calcFriction(const PylithScalar t,
						  const PylithScalar slip,
						  const PylithScalar slipRate,
						  const PylithScalar normalTraction,
						  const PylithScalar* properties,
						  const int numProperties,
						  const PylithScalar* stateVars,
						  const int numStateVars)
{ // _calcFriction
  // Check consistency of arguments.
  assert(properties);
  assert(_DoubleSlipWeakeningFrictionNoHeal::numProperties == numProperties);
  assert(numStateVars);
  assert(_DoubleSlipWeakeningFrictionNoHeal::numStateVars == numStateVars);

  // Compute friction traction.
  PylithScalar friction = 0.0;
  PylithScalar mu_f = 0.0;
  if (normalTraction <= 0.0) {
    // if fault is in compression
    const PylithScalar slipPrev = stateVars[s_slipPrev];
    const PylithScalar slipCum = stateVars[s_slipCum] + fabs(slip - slipPrev);

    if (slipCum < properties[p_distT]) {
	// if/else 
	mu_f = properties[p_coefS] -
	  (properties[p_coefS] - properties[p_coefT]) * 
	  slipCum / properties[p_distT];
	  } else if (slipCum < properties[p_distF]) {
	mu_f = properties[p_coefT] -
	  (properties[p_coefT] - properties[p_coefD]) * 
	  (slipCum - properties[p_distT]) / (properties[p_distF]-properties[p_distT]);
      } else {
	mu_f = properties[p_coefD];
      } // if/else
    friction = -mu_f * normalTraction + properties[p_cohesion];
  } else { // else
    friction = properties[p_cohesion];
  } // if/else

  PetscLogFlops(10);

  return friction;
} // _calcFriction

// ----------------------------------------------------------------------
// Compute derivative of friction with slip from properties and state variables.
PylithScalar
contrib::friction::DoubleSlipWeakeningFrictionNoHeal::_calcFrictionDeriv(const PylithScalar t,
						       const PylithScalar slip,
						       const PylithScalar slipRate,
						       const PylithScalar normalTraction,
						       const PylithScalar* properties,
						       const int numProperties,
						       const PylithScalar* stateVars,
						       const int numStateVars)
{ // _calcFrictionDeriv
  // Check consistency of arguments.
  assert(properties);
  assert(_DoubleSlipWeakeningFrictionNoHeal::numProperties == numProperties);
  assert(numStateVars);
  assert(_DoubleSlipWeakeningFrictionNoHeal::numStateVars == numStateVars);

  // Compute friction traction.
  PylithScalar frictionDeriv = 0.0;
  if (normalTraction <= 0.0) {
    // if fault is in compression
	const PylithScalar slipPrev = stateVars[s_slipPrev];
    const PylithScalar slipCum = stateVars[s_slipCum] + fabs(slip - slipPrev);

    if (slipCum < properties[p_distT]) {
      frictionDeriv = normalTraction * (properties[p_coefS] - properties[p_coefT]) / properties[p_distT];
    } else if (slipCum < properties[p_distF]) {
		frictionDeriv = normalTraction * (properties[p_coefT] - properties[p_coefD]) / (properties[p_distF]-properties[p_distT]);
      } // if/else
  } // if

  return frictionDeriv;
} // _calcFrictionDeriv

// ----------------------------------------------------------------------
// Update state variables (for next time step).
void
contrib::friction::DoubleSlipWeakeningFrictionNoHeal::_updateStateVars(const PylithScalar t,
						     const PylithScalar slip,
						     const PylithScalar slipRate,
						     const PylithScalar normalTraction,
						     PylithScalar* const stateVars,
						     const int numStateVars,
						     const PylithScalar* properties,
						     const int numProperties)
{ // _updateStateVars
  // Check consistency of arguments.
  assert(properties);
  assert(_DoubleSlipWeakeningFrictionNoHeal::numProperties == numProperties);
  assert(numStateVars);
  assert(_DoubleSlipWeakeningFrictionNoHeal::numStateVars == numStateVars);
  
  const PylithScalar tolerance = 0;
  if (slipRate >= tolerance) {
    const PylithScalar slipPrev = stateVars[s_slipPrev];

    stateVars[s_slipPrev] = slip;
    stateVars[s_slipCum] += fabs(slip - slipPrev);
  } else {
    // Sliding has stopped, so reset state variables.
    stateVars[s_slipPrev] = slip;
    stateVars[s_slipCum] = 0.0;
  } // else
	  
  //const PylithScalar slipPrev = stateVars[s_slipPrev];
  //stateVars[s_slipPrev] = slip;
  //stateVars[s_slipCum] += fabs(slip - slipPrev);
  
} // _updateStateVars


// End of file 