set(DOCUMENTATION
  "This module provides the Monteverdi program."
  )

otb_module( OTBMonteverdi
  DEPENDS
    OTBMonteverdiCore
    OTBMonteverdiGUI
    OTBQt4

  OPTIONAL_DEPENDS
  
  DESCRIPTION
  "${DOCUMENTATION}"
  )