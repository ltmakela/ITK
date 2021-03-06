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
#ifndef __itkPointSetToListSampleAdaptor_hxx
#define __itkPointSetToListSampleAdaptor_hxx

#include "itkPointSetToListSampleAdaptor.h"

namespace itk
{
namespace Statistics
{
template< class TPointSet >
PointSetToListSampleAdaptor< TPointSet >
::PointSetToListSampleAdaptor()
{
  m_PointSet = 0;
}

template< class TPointSet >
void
PointSetToListSampleAdaptor< TPointSet >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "PointSet: ";
  if ( m_PointSet.IsNotNull() )
    {
    os << m_PointSet << std::endl;
    }
  else
    {
    os << "not set." << std::endl;
    }
}

template< class TPointSet >
void
PointSetToListSampleAdaptor< TPointSet >
::SetPointSet(const TPointSet *pointSet)
{
  m_PointSet = pointSet;
  m_PointsContainer = pointSet->GetPoints();

  this->Modified();
}

template< class TPointSet >
const TPointSet *
PointSetToListSampleAdaptor< TPointSet >
::GetPointSet()
{
  if ( m_PointSet.IsNull() )
    {
    itkExceptionMacro("Point set has not been set yet");
    }

  return m_PointSet.GetPointer();
}

/** returns the number of measurement vectors in this container*/
template< class TPointSet >
typename PointSetToListSampleAdaptor< TPointSet >::InstanceIdentifier
PointSetToListSampleAdaptor< TPointSet >
::Size() const
{
  if ( m_PointSet.IsNull() )
    {
    itkExceptionMacro("Point set has not been set yet");
    }

  return m_PointsContainer->Size();
}

template< class TPointSet >
inline const typename PointSetToListSampleAdaptor< TPointSet >::MeasurementVectorType &
PointSetToListSampleAdaptor< TPointSet >
::GetMeasurementVector(InstanceIdentifier identifier) const
{
  if ( m_PointSet.IsNull() )
    {
    itkExceptionMacro("Point set has not been set yet");
    }

  m_PointSet->GetPoint(identifier, &m_TempPoint);
  return ( MeasurementVectorType & )m_TempPoint;
}

template< class TPointSet >
inline typename PointSetToListSampleAdaptor< TPointSet >::AbsoluteFrequencyType
PointSetToListSampleAdaptor< TPointSet >
::GetFrequency(InstanceIdentifier) const
{
  if ( m_PointSet.IsNull() )
    {
    itkExceptionMacro("Point set has not been set yet");
    }

  return 1;
}

template< class TPointSet >
typename PointSetToListSampleAdaptor< TPointSet >::TotalAbsoluteFrequencyType
PointSetToListSampleAdaptor< TPointSet >
::GetTotalFrequency() const
{
  if ( m_PointSet.IsNull() )
    {
    itkExceptionMacro("Point set has not been set yet");
    }

  return this->Size();
}
} // end of namespace Statistics
} // end of namespace itk

#endif
