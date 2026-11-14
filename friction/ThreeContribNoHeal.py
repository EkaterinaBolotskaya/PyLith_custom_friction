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

## @file pylith/friction/ThreeContribNoHeal.py
##
## @brief Python object implementing three-contribution friction without healing.
##
## Factory: friction_model.

# ISA FrictionModel
from pylith.friction.FrictionModel import FrictionModel

# Import the SWIG module ThreeContribNoHeal object and rename it
# ModuleThreeContribNoHeal so that it doesn't clash with the local Python
# class of the same name.
from frictioncontrib import ThreeContribNoHeal as ModuleThreeContribNoHeal

# ThreeContribNoHeal class
class ThreeContribNoHeal(FrictionModel, ModuleThreeContribNoHeal):
  """
  Python object implementing three-contribution friction without healing.

  Factory: friction_model.
  """
  # PUBLIC METHODS /////////////////////////////////////////////////////

  def __init__(self, name="ThreeContribNoHeal"):
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
           {'info': ["psi_p",
                     "u_dp",
                     "u_dhalf",
                     "k",
                     "mu_fpp",
                     "mu_fp",
                     "delta_u_fpp",
                     "mu_ss",
                     "delta_u_f",
                     "u_dam",
                     "mu_dp"],
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
    ModuleThreeContribNoHeal.__init__(self)
    return
  

# FACTORIES ////////////////////////////////////////////////////////////

# This is the function that is called when you invoke
# friction = pylith.pylith.contrib.ThreeContribNoHeal
# The name of this function MUST be 'friction_model'.
def friction_model():
  """
  Factory associated with ThreeContribNoHeal.
  """
  return ThreeContribNoHeal() # Return our object


# End of file 
