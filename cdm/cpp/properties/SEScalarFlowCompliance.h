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
#include "properties/SEScalar.h"
#include "bind/ScalarFlowComplianceData.hxx"

class DLL_DECL FlowComplianceUnit : public CCompoundUnit
{
public:
  FlowComplianceUnit(const std::string& u) : CCompoundUnit(u) {}
  virtual ~FlowComplianceUnit() {}

  static bool IsValidUnit(const std::string& unit);
  static const FlowComplianceUnit& GetCompoundUnit(const std::string& unit);

  static const FlowComplianceUnit L_Per_cmH2O;
  static const FlowComplianceUnit mL_Per_mmHg;
  static const FlowComplianceUnit mL_Per_cmH2O;
  static const FlowComplianceUnit m3_Per_Pa;
};

class DLL_DECL SEScalarFlowCompliance : public SEScalarQuantity<FlowComplianceUnit>
{
public:
  SEScalarFlowCompliance() {}
  virtual ~SEScalarFlowCompliance() {}

  CDM::ScalarFlowComplianceData* Unload() const;
};