/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
#include "stdafx.h"
#include "system/equipment/anesthesiamachine/actions/SEVaporizerFailure.h"
#include "properties/SEScalar0To1.h"

SEVaporizerFailure::SEVaporizerFailure() : SEAnesthesiaMachineAction()
{
  m_Severity = nullptr;
}

SEVaporizerFailure::~SEVaporizerFailure()
{
  Clear();
}

void SEVaporizerFailure::Clear()
{
  SEAnesthesiaMachineAction::Clear();
  SAFE_DELETE(m_Severity);
}

bool SEVaporizerFailure::IsValid() const
{
  return SEAnesthesiaMachineAction::IsValid() && HasSeverity();
}

bool SEVaporizerFailure::IsActive() const
{
  return HasSeverity() ? !m_Severity->IsZero() : false;
}

void SEVaporizerFailure::Load(const cdm::VaporizerFailureData& src, SEVaporizerFailure& dst)
{
  SEVaporizerFailure::Serialize(src, dst);
}
void SEVaporizerFailure::Serialize(const cdm::VaporizerFailureData& src, SEVaporizerFailure& dst)
{
  SEAnesthesiaMachineAction::Serialize(src.anesthesiamachineaction(), dst);
  if (src.has_severity())
    SEScalar0To1::Load(src.severity(), dst.GetSeverity());
}

cdm::VaporizerFailureData* SEVaporizerFailure::Unload(const SEVaporizerFailure& src)
{
  cdm::VaporizerFailureData* dst = new cdm::VaporizerFailureData();
  SEVaporizerFailure::Serialize(src, *dst);
  return dst;
}
void SEVaporizerFailure::Serialize(const SEVaporizerFailure& src, cdm::VaporizerFailureData& dst)
{
  SEAnesthesiaMachineAction::Serialize(src, *dst.mutable_anesthesiamachineaction());
  if (src.HasSeverity())
    dst.set_allocated_severity(SEScalar0To1::Unload(*src.m_Severity));
}

bool SEVaporizerFailure::HasSeverity() const
{
  return m_Severity != nullptr;
}
SEScalar0To1& SEVaporizerFailure::GetSeverity()
{
  if (m_Severity == nullptr)
    m_Severity = new SEScalar0To1();
  return *m_Severity;
}

void SEVaporizerFailure::ToString(std::ostream &str) const
{
  str << "Anesthesia Machine Action : Vaporizer Failure";
  if (HasComment())
    str << "\n\tComment: " << m_Comment;
  str << "\n\tSeverity :"; HasSeverity() ? str << *m_Severity : str << "NaN";
  str << std::flush;
}