/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkPointSetToPointSetMetric_hxx
#define __itkPointSetToPointSetMetric_hxx

#include "itkPointSetToPointSetMetric.h"

namespace itk
{
/** Constructor */
template< class TFixedPointSet, class TMovingPointSet >
PointSetToPointSetMetric< TFixedPointSet, TMovingPointSet >
::PointSetToPointSetMetric()
{
  m_FixedPointSet = 0;    // has to be provided by the user.
  m_MovingPointSet   = 0; // has to be provided by the user.
  m_Transform     = 0;    // has to be provided by the user.
}

/** Set the parameters that define a unique transform */
template< class TFixedPointSet, class TMovingPointSet >
void
PointSetToPointSetMetric< TFixedPointSet, TMovingPointSet >
::SetTransformParameters(const ParametersType & parameters) const
{
  if ( !m_Transform )
    {
    itkExceptionMacro(<< "Transform has not been assigned");
    }
  m_Transform->SetParameters(parameters);
}

/** Initialize the metric */
template< class TFixedPointSet, class TMovingPointSet >
void
PointSetToPointSetMetric< TFixedPointSet, TMovingPointSet >
::Initialize(void)
throw ( ExceptionObject )
{
  if ( !m_Transform )
    {
    itkExceptionMacro(<< "Transform is not present");
    }

  if ( !m_MovingPointSet )
    {
    itkExceptionMacro(<< "MovingPointSet is not present");
    }

  if ( !m_FixedPointSet )
    {
    itkExceptionMacro(<< "FixedPointSet is not present");
    }

  // If the PointSet is provided by a source, update the source.
  if ( m_MovingPointSet->GetSource() )
    {
    m_MovingPointSet->GetSource()->Update();
    }

  // If the point set is provided by a source, update the source.
  if ( m_FixedPointSet->GetSource() )
    {
    m_FixedPointSet->GetSource()->Update();
    }
}

/** PrintSelf */
template< class TFixedPointSet, class TMovingPointSet >
void
PointSetToPointSetMetric< TFixedPointSet, TMovingPointSet >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Moving PointSet: " << m_MovingPointSet.GetPointer()  << std::endl;
  os << indent << "Fixed  PointSet: " << m_FixedPointSet.GetPointer()   << std::endl;
  os << indent << "Transform:    " << m_Transform.GetPointer()    << std::endl;
}
} // end namespace itk

#endif
