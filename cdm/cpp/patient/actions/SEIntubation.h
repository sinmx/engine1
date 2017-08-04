/**************************************************************************************
Copyright 2015 Applied Research Associates, Inc.
Licensed under the Apache License, Version 2.0 (the "License"); you may not use
this file except in compliance with the License. You may obtain a copy of the License
at:
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.
**************************************************************************************/

#pragma once
#include "patient/actions/SEPatientAction.h"

class DLL_DECL SEIntubation : public SEPatientAction
{
public:

  SEIntubation();
  virtual ~SEIntubation();

  virtual void Clear(); //clear memory

  virtual bool IsValid() const;
  virtual bool IsActive() const;

  static void Load(const cdm::IntubationData& src, SEIntubation& dst);
  static cdm::IntubationData* Unload(const SEIntubation& src);
protected:
  static void Serialize(const cdm::IntubationData& src, SEIntubation& dst);
  static void Serialize(const SEIntubation& src, cdm::IntubationData& dst);

public:
    
  virtual cdm::IntubationData_eType GetType() const;
  virtual void SetType(cdm::IntubationData_eType t);

  virtual void ToString(std::ostream &str) const;
protected:
  cdm::IntubationData_eType m_Type;
};  