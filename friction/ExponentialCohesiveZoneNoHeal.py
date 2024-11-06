#!/usr/bin/env python
#
# ----------------------------------------------------------------------
#
# Brad T. Aagaard, U.S. Geological Survey
# Charles A. Williams, GNS Science
# Matthew G. Knepley, University of Chicago
#
# This code was developed as part of the Computational Infrastructure
# for Geodynamics (http://geodynamics.org).
#
# Copyright (c) 2010-2017 University of California, Davis
#
# See COPYING for license information.
#
# ----------------------------------------------------------------------
#

## @file pylith/friction/ExponentialCohesiveZoneNoHeal.py
##
## @brief Python object implementing exponential friction.
##
## Factory: friction_model.

# ISA FrictionModel
from pylith.friction.FrictionModel import FrictionModel

# Import the SWIG module ExponentialCohesiveZoneNoHeal object and rename it
# ModuleExponentialCohesiveZoneNoHeal so that it doesn't clash with the local Python
# class of the same name.
from frictioncontrib import ExponentialCohesiveZoneNoHeal as ModuleExponentialCohesiveZoneNoHeal

# ExponentialCohesiveZoneNoHeal class
class ExponentialCohesiveZoneNoHeal(FrictionModel, ModuleExponentialCohesiveZoneNoHeal):
  """
  Python object implementing exponential friction.

  Factory: friction_model.
  """
  # PUBLIC METHODS /////////////////////////////////////////////////////

  def __init__(self, name="ExponentialCohesiveZoneNoHeal"):
    """
    Constructor.
    """
    FrictionModel.__init__(self, name)
    # Set the fields that are available for output. These are the
    # stored physical properties and state variables. The friction
    # model information is output with the fault information, so we
    # can also output slip, slip rate and the fault tractions.
    #
    # There are no cell fields because the fault constitutive model
    # operations on quantities evaluated at the fault vertices.
    #
    # Do not change the name of this variable. The output manager will
    # request this variable by name.
    self.availableFields = \
        {'vertex': \
           {'info': ["static_coefficient",
		             "dynamic_coefficient",
                     "slip_shift",
	                 "slip_stretch",
                     "cohesion"],
            'data': ["cumulative_slip",
                     "previous_slip"]},
         'cell': \
           {'info': [],
            'data': []}}
    self._loggingPrefix = "FrDSlWk " # Prefix that appears in PETSc logging
    return


  # PRIVATE METHODS ////////////////////////////////////////////////////
  
  def _createModuleObj(self):
    """
    Call constructor for module object for access to C++ object. This
    function is called automatically by the generic Python FrictionModel
    object. It must have this name and self as the only argument.
    """
    ModuleExponentialCohesiveZoneNoHeal.__init__(self)
    return
  

# FACTORIES ////////////////////////////////////////////////////////////

# This is the function that is called when you invoke
# friction = pylith.pylith.contrib.ExponentialCohesiveZoneNoHeal
# The name of this function MUST be 'friction_model'.
def friction_model():
  """
  Factory associated with ExponentialCohesiveZoneNoHeal.
  """
  return ExponentialCohesiveZoneNoHeal() # Return our object


# End of file 
