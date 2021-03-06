/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#pragma once
#include "patient/assessments/SEPatientAssessment.h"
PROTO_PUSH
#include "bind/cdm/PatientAssessments.pb.h"
PROTO_POP

class CDM_DECL SEUrinalysisMicroscopic : public Loggable
{
  friend class SEUrinalysis;
protected:
  SEUrinalysisMicroscopic(Logger* logger);
public:
  virtual ~SEUrinalysisMicroscopic();

  virtual void Clear();

  static void Load(const cdm::UrinalysisData_UrinalysisMicroscopicData& src, SEUrinalysisMicroscopic& dst);
  static cdm::UrinalysisData_UrinalysisMicroscopicData* Unload(const SEUrinalysisMicroscopic& src);
protected:
  static void Serialize(const cdm::UrinalysisData_UrinalysisMicroscopicData& src, SEUrinalysisMicroscopic& dst);
  static void Serialize(const SEUrinalysisMicroscopic& src, cdm::UrinalysisData_UrinalysisMicroscopicData& dst);

public:

  virtual bool HasObservationType() const;  
  virtual cdm::UrinalysisData_eMicroscopicObservationType GetObservationType() const;
  virtual void SetObservationType(cdm::UrinalysisData_eMicroscopicObservationType p);
  virtual void InvalidateObservationType();

  virtual bool HasRedBloodCellsResult() const;
  virtual SEScalarAmount& GetRedBloodCellsResult();
  virtual double GetRedBloodCellsResult(const AmountUnit& unit) const;

  virtual bool HasWhiteBloodCellsResult() const;
  virtual SEScalarAmount& GetWhiteBloodCellsResult();
  virtual double GetWhiteBloodCellsResult(const AmountUnit& unit) const;

  virtual bool HasEpithelialCellsResult() const;
  virtual cdm::UrinalysisData_eMicroscopicObservationAmount GetEpithelialCellsResult() const;
  virtual void SetEpithelialCellsResult(cdm::UrinalysisData_eMicroscopicObservationAmount p);
  virtual void InvalidateEpithelialCellsResult();

  virtual bool HasCastsResult() const;
  virtual SEScalarAmount& GetCastsResult();
  virtual double GetCastsResult(const AmountUnit& unit) const;

  virtual bool HasCrystalsResult() const;
  virtual cdm::UrinalysisData_eMicroscopicObservationAmount GetCrystalsResult() const;
  virtual void SetCrystalsResult(cdm::UrinalysisData_eMicroscopicObservationAmount p);
  virtual void InvalidateCrystalsResult();

  virtual bool HasBacteriaResult() const;
  virtual cdm::UrinalysisData_eMicroscopicObservationAmount GetBacteriaResult() const;
  virtual void SetBacteriaResult(cdm::UrinalysisData_eMicroscopicObservationAmount p);
  virtual void InvalidateBacteriaResult();

  virtual bool HasTrichomonadsResult() const;
  virtual cdm::UrinalysisData_eMicroscopicObservationAmount GetTrichomonadsResult() const;
  virtual void SetTrichomonadsResult(cdm::UrinalysisData_eMicroscopicObservationAmount p);
  virtual void InvalidateTrichomonadsResult();

  virtual bool HasYeastResult() const;
  virtual cdm::UrinalysisData_eMicroscopicObservationAmount GetYeastResult() const;
  virtual void SetYeastResult(cdm::UrinalysisData_eMicroscopicObservationAmount p);
  virtual void InvalidateYeastResult();

protected:

  cdm::UrinalysisData_eMicroscopicObservationType   m_ObservationType;
  SEScalarAmount*                                   m_RedBloodCells;
  SEScalarAmount*                                   m_WhiteBloodCells;
  cdm::UrinalysisData_eMicroscopicObservationAmount m_EpithelialCells;
  SEScalarAmount*                                   m_Casts;
  cdm::UrinalysisData_eMicroscopicObservationAmount m_Crystals;
  cdm::UrinalysisData_eMicroscopicObservationAmount m_Bacteria;
  cdm::UrinalysisData_eMicroscopicObservationAmount m_Trichomonads;
  cdm::UrinalysisData_eMicroscopicObservationAmount m_Yeast;
};  