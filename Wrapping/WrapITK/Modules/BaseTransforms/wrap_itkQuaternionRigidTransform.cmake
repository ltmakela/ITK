FILTER_DIMS(d3 3)
if(d3)
  WRAP_CLASS("itk::QuaternionRigidTransform" POINTER)
    WRAP_TEMPLATE("${ITKM_D}" "${ITKT_D}")
  END_WRAP_CLASS()
endif(d3)
