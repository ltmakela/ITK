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
#ifndef __itkStringStream_h
#define __itkStringStream_h

// Need to include at least one ITK header.
#include "itkMacro.h"


namespace itk
{

/** \class StringStream
 *  \brief Provides access to C++ ostreams.
 */
class StringStream: public std::ostringstream
{
public:
  typedef StringStream       Self;
  typedef std::ostringstream Superclass;

  StringStream();
  ~StringStream();
  std::ostream& GetStream() { return *this;}
  const char* GetString();
  void Reset();
private:
  std::string m_String;
  StringStream(const StringStream&); // Not implemented.
  void operator=(const StringStream&); // Not implemented.

};

}

#endif
