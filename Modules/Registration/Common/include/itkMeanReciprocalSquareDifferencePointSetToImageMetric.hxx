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
#ifndef __itkMeanReciprocalSquareDifferencePointSetToImageMetric_hxx
#define __itkMeanReciprocalSquareDifferencePointSetToImageMetric_hxx

#include "itkMeanReciprocalSquareDifferencePointSetToImageMetric.h"
#include "itkImageRegionConstIteratorWithIndex.h"

namespace itk
{
/*
* Constructor
*/
template <class TFixedPointSet, class TMovingImage>
MeanReciprocalSquareDifferencePointSetToImageMetric<TFixedPointSet, TMovingImage>
::MeanReciprocalSquareDifferencePointSetToImageMetric()
{
  m_Lambda = 1.0;
}

/**
 * Get the match Measure
 */
template <class TFixedPointSet, class TMovingImage>
typename MeanReciprocalSquareDifferencePointSetToImageMetric<TFixedPointSet, TMovingImage>::MeasureType
MeanReciprocalSquareDifferencePointSetToImageMetric<TFixedPointSet, TMovingImage>
::GetValue(const TransformParametersType & parameters) const
{
  FixedPointSetConstPointer fixedPointSet = this->GetFixedPointSet();

  if( !fixedPointSet )
    {
    itkExceptionMacro(<< "Fixed point set has not been assigned");
    }

  PointIterator pointItr = fixedPointSet->GetPoints()->Begin();
  PointIterator pointEnd = fixedPointSet->GetPoints()->End();

  PointDataIterator pointDataItr = fixedPointSet->GetPointData()->Begin();
  PointDataIterator pointDataEnd = fixedPointSet->GetPointData()->End();

  MeasureType measure = NumericTraits<MeasureType>::Zero;

  this->m_NumberOfPixelsCounted = 0;
  double lambdaSquared = vcl_pow(this->m_Lambda, 2);

  this->SetTransformParameters(parameters);

  typedef  typename NumericTraits<MeasureType>::AccumulateType AccumulateType;

  while( pointItr != pointEnd && pointDataItr != pointDataEnd )
    {
    InputPointType inputPoint;
    inputPoint.CastFrom( pointItr.Value() );
    OutputPointType transformedPoint =
      this->m_Transform->TransformPoint(inputPoint);

    if( this->m_Interpolator->IsInsideBuffer(transformedPoint) )
      {
      const RealType movingValue  = this->m_Interpolator->Evaluate(transformedPoint);
      const RealType fixedValue   = pointDataItr.Value();
      const RealType diff = movingValue - fixedValue;
      const double   diffSquared = diff * diff;
      measure += 1.0 / ( lambdaSquared + diffSquared );
      this->m_NumberOfPixelsCounted++;
      }

    ++pointItr;
    ++pointDataItr;
    }

  if( !this->m_NumberOfPixelsCounted )
    {
    itkExceptionMacro(<< "All the points mapped to outside of the moving image");
    }
  else
    {
    measure *= ( lambdaSquared / this->m_NumberOfPixelsCounted );
    }

  return measure;
}

/*
 * Get the Derivative Measure
 */
template <class TFixedPointSet, class TMovingImage>
void
MeanReciprocalSquareDifferencePointSetToImageMetric<TFixedPointSet, TMovingImage>
::GetDerivative(const TransformParametersType & parameters,
                DerivativeType & derivative) const
{
  if( !this->GetGradientImage() )
    {
    itkExceptionMacro(<< "The gradient image is null, maybe you forgot to call Initialize()");
    }

  FixedPointSetConstPointer fixedPointSet = this->GetFixedPointSet();

  if( !fixedPointSet )
    {
    itkExceptionMacro(<< "Fixed image has not been assigned");
    }

  this->m_NumberOfPixelsCounted = 0;

  double lambdaSquared = vcl_pow(this->m_Lambda, 2);

  this->SetTransformParameters(parameters);

  typedef  typename NumericTraits<MeasureType>::AccumulateType AccumulateType;

  const unsigned int ParametersDimension = this->GetNumberOfParameters();
  derivative = DerivativeType(ParametersDimension);
  derivative.Fill(NumericTraits<typename DerivativeType::ValueType>::Zero);

  PointIterator pointItr = fixedPointSet->GetPoints()->Begin();
  PointIterator pointEnd = fixedPointSet->GetPoints()->End();

  PointDataIterator pointDataItr = fixedPointSet->GetPointData()->Begin();
  PointDataIterator pointDataEnd = fixedPointSet->GetPointData()->End();

  TransformJacobianType jacobian;

  while( pointItr != pointEnd && pointDataItr != pointDataEnd )
    {
    InputPointType inputPoint;
    inputPoint.CastFrom( pointItr.Value() );
    OutputPointType transformedPoint =
      this->m_Transform->TransformPoint(inputPoint);

    if( this->m_Interpolator->IsInsideBuffer(transformedPoint) )
      {
      const RealType movingValue  = this->m_Interpolator->Evaluate(transformedPoint);
      const RealType fixedValue   = pointDataItr.Value();

      this->m_NumberOfPixelsCounted++;
      const RealType diff = movingValue - fixedValue;
      const RealType diffSquared = diff * diff;

      // Now compute the derivatives
      this->m_Transform->ComputeJacobianWithRespectToParameters(inputPoint, jacobian);

      // Get the gradient by NearestNeighboorInterpolation:
      // which is equivalent to round up the point components.
      typedef typename OutputPointType::CoordRepType CoordRepType;
      typedef ContinuousIndex<CoordRepType, MovingImageType::ImageDimension>
      MovingImageContinuousIndexType;

      MovingImageContinuousIndexType tempIndex;
      this->m_MovingImage->TransformPhysicalPointToContinuousIndex(transformedPoint, tempIndex);

      typename MovingImageType::IndexType mappedIndex;
      mappedIndex.CopyWithRound(tempIndex);

      const GradientPixelType gradient =
        this->GetGradientImage()->GetPixel(mappedIndex);
      for( unsigned int par = 0; par < ParametersDimension; par++ )
        {
        RealType sum = NumericTraits<RealType>::Zero;
        for( unsigned int dim = 0; dim < Self::FixedPointSetDimension; dim++ )
          {
          // Will it be computationally more efficient to instead calculate the
          // derivative using finite differences ?
          sum -= jacobian(dim, par)
            * gradient[dim] / ( vcl_pow(lambdaSquared + diffSquared, 2) );
          }
        derivative[par] += diff * sum;
        }
      }

    ++pointItr;
    ++pointDataItr;
    }

  if( !this->m_NumberOfPixelsCounted )
    {
    itkExceptionMacro(<< "All the points mapped to outside of the moving image");
    }
  else
    {
    for( unsigned int i = 0; i < ParametersDimension; i++ )
      {
      derivative[i] *= 2.0 * lambdaSquared / this->m_NumberOfPixelsCounted;
      }
    }
}

/*
 * Get both the match Measure and theDerivative Measure
 */
template <class TFixedPointSet, class TMovingImage>
void
MeanReciprocalSquareDifferencePointSetToImageMetric<TFixedPointSet, TMovingImage>
::GetValueAndDerivative(const TransformParametersType & parameters,
                        MeasureType & value, DerivativeType  & derivative) const
{
  if( !this->GetGradientImage() )
    {
    itkExceptionMacro(<< "The gradient image is null, maybe you forgot to call Initialize()");
    }

  FixedPointSetConstPointer fixedPointSet = this->GetFixedPointSet();

  if( !fixedPointSet )
    {
    itkExceptionMacro(<< "Fixed image has not been assigned");
    }

  this->m_NumberOfPixelsCounted = 0;
  MeasureType measure = NumericTraits<MeasureType>::Zero;

  this->SetTransformParameters(parameters);
  double lambdaSquared = vcl_pow(this->m_Lambda, 2);

  typedef  typename NumericTraits<MeasureType>::AccumulateType AccumulateType;

  const unsigned int ParametersDimension = this->GetNumberOfParameters();
  derivative = DerivativeType(ParametersDimension);
  derivative.Fill(NumericTraits<typename DerivativeType::ValueType>::Zero);

  PointIterator pointItr = fixedPointSet->GetPoints()->Begin();
  PointIterator pointEnd = fixedPointSet->GetPoints()->End();

  PointDataIterator pointDataItr = fixedPointSet->GetPointData()->Begin();
  PointDataIterator pointDataEnd = fixedPointSet->GetPointData()->End();

  while( pointItr != pointEnd && pointDataItr != pointDataEnd )
    {
    InputPointType inputPoint;
    inputPoint.CastFrom( pointItr.Value() );
    OutputPointType transformedPoint =
      this->m_Transform->TransformPoint(inputPoint);

    if( this->m_Interpolator->IsInsideBuffer(transformedPoint) )
      {
      const RealType movingValue  = this->m_Interpolator->Evaluate(transformedPoint);
      const RealType fixedValue   = pointDataItr.Value();

      this->m_NumberOfPixelsCounted++;

      // Now compute the derivatives
      TransformJacobianType jacobian;
      this->m_Transform->ComputeJacobianWithRespectToParameters(inputPoint, jacobian);

      const RealType diff = movingValue - fixedValue;
      const RealType diffSquared = diff * diff;
      measure += 1.0 / ( lambdaSquared + diffSquared );

      // Get the gradient by NearestNeighboorInterpolation:
      // which is equivalent to round up the point components.
      typedef typename OutputPointType::CoordRepType CoordRepType;
      typedef ContinuousIndex<CoordRepType, MovingImageType::ImageDimension>
      MovingImageContinuousIndexType;

      MovingImageContinuousIndexType tempIndex;
      this->m_MovingImage->TransformPhysicalPointToContinuousIndex(transformedPoint, tempIndex);

      typename MovingImageType::IndexType mappedIndex;
      mappedIndex.CopyWithRound(tempIndex);

      const GradientPixelType gradient =
        this->GetGradientImage()->GetPixel(mappedIndex);
      for( unsigned int par = 0; par < ParametersDimension; par++ )
        {
        RealType sum = NumericTraits<RealType>::Zero;
        for( unsigned int dim = 0; dim < Self::FixedPointSetDimension; dim++ )
          {
          sum -= jacobian(dim, par) * gradient[dim]
            * vcl_pow(lambdaSquared + diffSquared, 2);
          }
        derivative[par] += diff * sum;
        }
      }

    ++pointItr;
    ++pointDataItr;
    }

  if( !this->m_NumberOfPixelsCounted )
    {
    itkExceptionMacro(<< "All the points mapped to outside of the moving image");
    }
  else
    {
    for( unsigned int i = 0; i < ParametersDimension; i++ )
      {
      derivative[i] *= 2.0 * lambdaSquared / this->m_NumberOfPixelsCounted;
      }
    measure *= lambdaSquared / this->m_NumberOfPixelsCounted;
    }

  value = measure;
}

/**
 * PrintSelf
 */
template <class TFixedPointSet, class TMovingImage>
void
MeanReciprocalSquareDifferencePointSetToImageMetric<TFixedPointSet, TMovingImage>
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << "Lambda factor = " << m_Lambda << std::endl;
}

} // end namespace itk

#endif
