/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/


#include "stdafx.h"
#include "AnesthesiaMachine.h"

#include "circuit/fluid/SEFluidCircuit.h"
#include "compartment/substances/SEGasSubstanceQuantity.h"
#include "substance/SESubstance.h"
#include "properties/SEScalar0To1.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarFrequency.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarVolumePerTime.h"
#include "properties/SEScalar0To1.h"
#include "properties/SEScalarFlowResistance.h"
#include "system/equipment/anesthesiamachine/SEAnesthesiaMachineOxygenBottle.h"
#include "system/equipment/anesthesiamachine/SEAnesthesiaMachineChamber.h"

/*
========================
  Constructors
========================
*/

AnesthesiaMachine::AnesthesiaMachine(PulseController& data) : SEAnesthesiaMachine(data.GetSubstances()), m_data(data)
{
  Clear();
}

AnesthesiaMachine::~AnesthesiaMachine ()
{
  Clear();
}

void AnesthesiaMachine::Clear()
{
  SEAnesthesiaMachine::Clear();
  m_actions = nullptr;
  m_ambient = nullptr;
  m_ambientCO2 = nullptr;
  m_ambientN2 = nullptr;
  m_ambientO2 = nullptr;
  m_gasSource = nullptr;
  m_gasSourceCO2 = nullptr;
  m_gasSourceN2 = nullptr;
  m_gasSourceO2 = nullptr;
  m_scrubber = nullptr;
  m_scubberCO2 = nullptr;
  m_scrubberN2 = nullptr;
  m_pAnesthesiaConnectionToEnvironment = nullptr;
  m_pYPieceToExpiratoryLimb = nullptr;
  m_pGasSourceToGasInlet = nullptr;
  m_pInspiratoryLimbToYPiece = nullptr;
  m_pSelectorToReliefValve = nullptr;
  m_pEnvironmentToReliefValve = nullptr;
  m_pSelectorToEnvironment = nullptr;
  m_pEnvironmentToVentilator = nullptr;
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Initializes system properties to valid homeostatic values.
//--------------------------------------------------------------------------------------------------
void AnesthesiaMachine::Initialize()
{
  PulseSystem::Initialize();

  SetConnection(cdm::AnesthesiaMachineData_eConnection_Off);
  GetInletFlow().SetValue(5.0, VolumePerTimeUnit::L_Per_min);
  GetRespiratoryRate().SetValue(12.0, FrequencyUnit::Per_min);
  GetPositiveEndExpiredPressure().SetValue(3.0, PressureUnit::cmH2O);
  GetInspiratoryExpiratoryRatio().SetValue(0.5);
  GetOxygenFraction().SetValue(0.5);
  SetOxygenSource(cdm::AnesthesiaMachineData_eOxygenSource_Wall);
  SetPrimaryGas(cdm::AnesthesiaMachineData_ePrimaryGas_Nitrogen);
  GetVentilatorPressure().SetValue(15.0, PressureUnit::cmH2O);
  GetOxygenBottleOne().GetVolume().SetValue(660.0, VolumeUnit::L);
  GetOxygenBottleTwo().GetVolume().SetValue(660.0, VolumeUnit::L);
  GetReliefValvePressure().SetValue(100.0, PressureUnit::cmH2O);

  m_inhaling = true;
  m_inspirationTime.SetValue(0.0, TimeUnit::s);
  m_O2InletVolumeFraction.SetValue(0.0);
  m_totalBreathingCycleTime.SetValue(0.0, TimeUnit::s);
  m_currentbreathingCycleTime.SetValue(0.0, TimeUnit::s);
  
  StateChange();
}

void AnesthesiaMachine::Load(const pulse::AnesthesiaMachineData& src, AnesthesiaMachine& dst)
{
  AnesthesiaMachine::Serialize(src, dst);
  dst.SetUp();
}
void AnesthesiaMachine::Serialize(const pulse::AnesthesiaMachineData& src, AnesthesiaMachine& dst)
{
  SEAnesthesiaMachine::Serialize(src.common(), dst);
  dst.m_inhaling = src.inhaling();
  dst.m_currentbreathingCycleTime.SetValue(src.currentbreathingcycletime_s(),TimeUnit::s);
  dst.m_inspirationTime.SetValue(src.inspirationtime_s(), TimeUnit::s);
  dst.m_O2InletVolumeFraction.SetValue(src.oxygeninletvolumefraction());
  dst.m_totalBreathingCycleTime.SetValue(src.totalbreathingcycletime_s(), TimeUnit::s);
}

pulse::AnesthesiaMachineData* AnesthesiaMachine::Unload(const AnesthesiaMachine& src)
{
  pulse::AnesthesiaMachineData* dst = new pulse::AnesthesiaMachineData();
  AnesthesiaMachine::Serialize(src, *dst);
  return dst;
}
void AnesthesiaMachine::Serialize(const AnesthesiaMachine& src, pulse::AnesthesiaMachineData& dst)
{
  SEAnesthesiaMachine::Serialize(src, *dst.mutable_common());
  dst.set_inhaling(src.m_inhaling);
  dst.set_currentbreathingcycletime_s(src.m_currentbreathingCycleTime.GetValue(TimeUnit::s));
  dst.set_inspirationtime_s(src.m_inspirationTime.GetValue(TimeUnit::s));
  dst.set_oxygeninletvolumefraction(src.m_O2InletVolumeFraction.GetValue());
  dst.set_totalbreathingcycletime_s(src.m_totalBreathingCycleTime.GetValue(TimeUnit::s));
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Initializes parameters for the anesthesia machine class
///
/// \details
/// Initializes member variables and system level values on the common data model.
//--------------------------------------------------------------------------------------------------
void AnesthesiaMachine::SetUp()
{
  m_dt_s = m_data.GetTimeStep().GetValue(TimeUnit::s);
  m_actions = &m_data.GetActions().GetAnesthesiaMachineActions();
  m_dValveOpenResistance_cmH2O_s_Per_L = m_data.GetConfiguration().GetMachineOpenResistance(FlowResistanceUnit::cmH2O_s_Per_L);
  m_dValveClosedResistance_cmH2O_s_Per_L = m_data.GetConfiguration().GetMachineClosedResistance(FlowResistanceUnit::cmH2O_s_Per_L);
  m_dSwitchOpenResistance_cmH2O_s_Per_L = m_data.GetConfiguration().GetDefaultOpenFlowResistance(FlowResistanceUnit::cmH2O_s_Per_L);
  m_dSwitchClosedResistance_cmH2O_s_Per_L = m_data.GetConfiguration().GetDefaultClosedFlowResistance(FlowResistanceUnit::cmH2O_s_Per_L);

  // Compartments
  m_ambient = m_data.GetCompartments().GetGasCompartment(pulse::EnvironmentCompartment::Ambient);
  m_ambientCO2 = m_ambient->GetSubstanceQuantity(m_data.GetSubstances().GetCO2());
  m_ambientN2 = m_ambient->GetSubstanceQuantity(m_data.GetSubstances().GetN2());
  m_ambientO2 = m_ambient->GetSubstanceQuantity(m_data.GetSubstances().GetO2());

  m_gasSource = m_data.GetCompartments().GetGasCompartment(pulse::AnesthesiaMachineCompartment::GasSource);
  m_gasSourceCO2 = m_gasSource->GetSubstanceQuantity(m_data.GetSubstances().GetCO2());
  m_gasSourceN2 = m_gasSource->GetSubstanceQuantity(m_data.GetSubstances().GetN2());
  m_gasSourceO2 = m_gasSource->GetSubstanceQuantity(m_data.GetSubstances().GetO2());

  m_scrubber = m_data.GetCompartments().GetGasCompartment(pulse::AnesthesiaMachineCompartment::Scrubber);
  m_scubberCO2 = m_scrubber->GetSubstanceQuantity(m_data.GetSubstances().GetCO2());
  m_scrubberN2 = m_scrubber->GetSubstanceQuantity(m_data.GetSubstances().GetN2());

  // Circuit Nodes
  m_nVentilator = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetNode(pulse::AnesthesiaMachineNode::Ventilator);

  // Circuit Paths
  m_pAnesthesiaConnectionToEnvironment = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::AnesthesiaConnectionToEnvironment);
  m_pYPieceToExpiratoryLimb = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::YPieceToExpiratoryLimb);
  m_pGasSourceToGasInlet = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::GasSourceToGasInlet);
  m_pInspiratoryLimbToYPiece = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::InspiratoryLimbToYPiece);
  m_pSelectorToReliefValve = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::SelectorToReliefValve);
  m_pEnvironmentToReliefValve = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::EnvironmentToReliefValve);
  m_pSelectorToEnvironment = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::SelectorToEnvironment);
  m_pEnvironmentToVentilator = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::EnvironmentToVentilator);
  m_pExpiratoryLimbToSelector = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::ExpiratoryLimbToSelector);
  m_pSelectorToScrubber = m_data.GetCircuits().GetAnesthesiaMachineCircuit().GetPath(pulse::AnesthesiaMachinePath::SelectorToScrubber);
}

void AnesthesiaMachine::StateChange()
{
  if (HasLeftChamber() && GetLeftChamber().GetState() == cdm::eSwitch::On && GetLeftChamber().HasSubstance())
    m_Substances.AddActiveSubstance(*m_LeftChamber->GetSubstance());
  if (HasRightChamber() && GetRightChamber().GetState() == cdm::eSwitch::On && GetRightChamber().HasSubstance())
    m_Substances.AddActiveSubstance(*m_RightChamber->GetSubstance());
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Connect to the patient via the ventilator mask, an endotracheal tube, or no connection
///
/// \param  enumAnesthesiaMachineConnection  
/// Connectoin type : Mask, tube, or off
/// 
/// \details
/// If the enum is set to mask, then the mask is applied to the face
/// If the enum is set to tube, then the machine is connected to the tube
/// If the enum is set to off, the airway mode is set to free.
//--------------------------------------------------------------------------------------------------
void AnesthesiaMachine::SetConnection(cdm::AnesthesiaMachineData_eConnection c)
{
  if (m_Connection == c)
    return; // No Change
  // Update the Pulse airway mode when this changes
  SEAnesthesiaMachine::SetConnection(c);
  if (c == cdm::AnesthesiaMachineData_eConnection_Mask && m_data.GetIntubation() == cdm::eSwitch::Off)
  {
    m_data.SetAirwayMode(pulse::eAirwayMode::AnesthesiaMachine);
    return;
  }
  else if (c == cdm::AnesthesiaMachineData_eConnection_Tube && m_data.GetIntubation() == cdm::eSwitch::On)
  {
    m_data.SetAirwayMode(pulse::eAirwayMode::AnesthesiaMachine);
    return;
  }
  else if (c == cdm::AnesthesiaMachineData_eConnection_Mask && m_data.GetIntubation() == cdm::eSwitch::On)
    Error("Connection failed : Cannot apply anesthesia machine mask if patient is intubated.");
  else if (c == cdm::AnesthesiaMachineData_eConnection_Tube && m_data.GetIntubation() == cdm::eSwitch::Off)
    Error("Connection failed : Cannot apply anesthesia machine to tube if patient is not intubated.");
  // Make sure we are active to make sure we go back to free
  m_data.SetAirwayMode(pulse::eAirwayMode::Free);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Removes the connection to the patient
///
/// \details
/// If the mask is on or the tube is connected, it is removed and the airway mode is set to free. The action is then removed from 
/// the action manager.
//--------------------------------------------------------------------------------------------------
void AnesthesiaMachine::InvalidateConnection()
{
  // Set airway mode to free
  m_data.SetAirwayMode(pulse::eAirwayMode::Free);
  // THEN invalidate
  m_Connection = cdm::AnesthesiaMachineData_eConnection_Off;
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Initializes gas volumes and volume fractions supplied by the anesthesia machine depending on the airway mode
///
/// \details
/// The gas volumes and volume fractions are initialized and updated based on the airway mode (mask, free, or tube)
/// and the volume associated with each airway mode.
//--------------------------------------------------------------------------------------------------
void AnesthesiaMachine::SetConnection()
{
  switch (m_data.GetAirwayMode())
  {
  case pulse::eAirwayMode::Free:
    //Basically a full leak to ground
    m_pAnesthesiaConnectionToEnvironment->GetNextResistance().SetValue(m_dSwitchClosedResistance_cmH2O_s_Per_L, FlowResistanceUnit::cmH2O_s_Per_L);
    break;
  case pulse::eAirwayMode::AnesthesiaMachine:
    if (m_Connection == cdm::AnesthesiaMachineData_eConnection_Mask)
    {
      if (m_data.GetIntubation() == cdm::eSwitch::On)// Somebody intubated while we had the mask on
      {
        Info("Anesthesia Machine has been disconnected due to an intubation.");
        m_data.SetAirwayMode(pulse::eAirwayMode::Free);
        return;
      }

      //Keep the baseline resistance to ground = an open switch
      //Leaks handled later:L);
    }
    else if (m_Connection == cdm::AnesthesiaMachineData_eConnection_Tube)
    {
      if (m_data.GetIntubation() == cdm::eSwitch::Off)// Somebody removed intubated while we were connected to it
      {
        Info("Anesthesia Machine has been disconnected removal of intubation.");
        m_data.SetAirwayMode(pulse::eAirwayMode::Free);
        return;
      }

      //Keep the baseline resistance to ground = an open switch
      //Leaks handled later:L);
    }
    break;
  default:
    Fatal("Unhandled Airway Mode.");
  }
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Anesthesia machine preprocess function
///
/// \details
/// Checks the anesthesia machine settings to ensure all are within the acceptable ranges. 
/// Processes all equipment failures.
/// Verifies the oxygen source.
/// Processes any failures with the oxygen source. 
/// Calculates the gas inlet volume fractions based on equipment failures and gas sources.
//--------------------------------------------------------------------------------------------------
void AnesthesiaMachine::PreProcess()
{
  if (m_data.GetActions().GetAnesthesiaMachineActions().HasConfiguration())
  {
    ProcessConfiguration(*m_data.GetActions().GetAnesthesiaMachineActions().GetConfiguration());
    m_data.GetActions().GetAnesthesiaMachineActions().RemoveConfiguration();
  }
  //Do nothing if the machine is off and not initialized
  if (GetConnection() == cdm::AnesthesiaMachineData_eConnection_Off)
  {
    m_inhaling = true;
    m_currentbreathingCycleTime.SetValue(0.0, TimeUnit::s);
    return;
  }

  CalculateCyclePhase();
  CalculateSourceStatus();
  SetConnection();
  CalculateValveResistances();
  CalculateEquipmentLeak();
  CalculateVentilator();
  CalculateGasSource();
  CheckReliefValve();  
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Anesthesia machine process function
///
/// \details
/// The current Pulse implementation has no specific process functionality for the anesthesia machine.
/// Anesthesia machine processing is currently done in the Respiratory System with the combined circuit methodology.
//--------------------------------------------------------------------------------------------------
void AnesthesiaMachine::Process()
{
  if (GetConnection() != cdm::AnesthesiaMachineData_eConnection_Off)
  {
    CalculateScrubber();
  }
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Anesthesia machine postprocess function
///
/// \details
/// The substance volumes and the volume fractions are updated for all of the nodes in the anesthesia machine
/// circuit during post process. 
/// The ventilator volumes are updated based on the previously calculated nodal analysis.
//--------------------------------------------------------------------------------------------------
void AnesthesiaMachine::PostProcess()
{
  UpdateEvents(m_data.GetTimeStep());
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculates the carbon dioxide removed by the scrubber
///
/// \details
/// Calculates the amount of carbon dioxide removed from the anesthesia machine by the scrubber. If a soda lime 
/// failure is present, the full amount of carbon dioxide is not removed. The amount removed is dependent on the 
/// severity of the failure. The substance volume in the scrubber node is decremented based on the amount of 
/// carbon dioxide removed. This function occurs during anesthesia machine preprocess.
//--------------------------------------------------------------------------------------------------
void AnesthesiaMachine::CalculateScrubber()
{  
  // Calculate incremental volume changes due to scrubber
  double CO2Volume = 0.0;  

  if (m_data.GetActions().GetAnesthesiaMachineActions().HasSodaLimeFailure())
  {
    double SodaLimeFailSeverity = m_actions->GetSodaLimeFailure()->GetSeverity().GetValue();
    CO2Volume = SodaLimeFailSeverity * m_scubberCO2->GetVolume(VolumeUnit::L);
  }

  double CO2PreviousVolume = m_scubberCO2->GetVolume(VolumeUnit::L);
  m_scrubberN2->GetVolume().IncrementValue(CO2PreviousVolume - CO2Volume, VolumeUnit::L);
  m_scubberCO2->GetVolume().SetValue(CO2Volume, VolumeUnit::L);
  m_scrubber->Balance(BalanceGasBy::Volume);
}  

//--------------------------------------------------------------------------------------------------
/// \brief
/// Updates the volume fractions for the gas inlet node
///
/// \details
/// The final step of the anesthesia machine preprocess is to update the volume fractions for the gas inlet node.
/// The gas mixture is determined based on the right and left chamber activity and specified substances and the oxygen percent
/// setting on the anesthesia machine. The oxygen source (bottle and wall) are checked to ensure no equipment failures exist. 
/// The volume fractions are adjusted according to gas composition, the sources, and any failures present. 
//--------------------------------------------------------------------------------------------------
void AnesthesiaMachine::CalculateGasSource()
{
  double dInletflow = GetInletFlow().GetValue(VolumePerTimeUnit::L_Per_min);
  m_pGasSourceToGasInlet->GetNextFlowSource().SetValue(dInletflow, VolumePerTimeUnit::L_Per_min);
  
  //For Exhaust to balance volume properly
  m_pSelectorToEnvironment->GetNextFlowSource().SetValue(dInletflow, VolumePerTimeUnit::L_Per_min);
  
  double LeftInhaledAgentVolumeFraction = 0.0;
  double RightInhaledAgentVolumeFraction = 0.0;
  //Vaporizer Failure
  double VaporizerFailureSeverity = 0.0;

  if(m_data.GetActions().GetAnesthesiaMachineActions().HasVaporizerFailure()) 
  {
    VaporizerFailureSeverity = m_data.GetActions().GetAnesthesiaMachineActions().GetVaporizerFailure()->GetSeverity().GetValue();
  }
  if(GetLeftChamber().GetState() == cdm::eSwitch::On && GetLeftChamber().HasSubstance())
  {
    SEGasSubstanceQuantity* gasSrcSubQ = m_gasSource->GetSubstanceQuantity(*GetLeftChamber().GetSubstance());
    LeftInhaledAgentVolumeFraction = GetLeftChamber().GetSubstanceFraction().GetValue();
    LeftInhaledAgentVolumeFraction = LeftInhaledAgentVolumeFraction * (1-VaporizerFailureSeverity);
    gasSrcSubQ->GetVolumeFraction().SetValue(LeftInhaledAgentVolumeFraction);
  }
  if(GetRightChamber().GetState() == cdm::eSwitch::On && GetRightChamber().HasSubstance())
  {
    SEGasSubstanceQuantity* gasSrcSubQ = m_gasSource->GetSubstanceQuantity(*GetRightChamber().GetSubstance());
    RightInhaledAgentVolumeFraction = GetRightChamber().GetSubstanceFraction().GetValue();
    RightInhaledAgentVolumeFraction = RightInhaledAgentVolumeFraction * (1-VaporizerFailureSeverity);
    gasSrcSubQ->GetVolumeFraction().SetValue(RightInhaledAgentVolumeFraction);
  }

  double RemainingVolumeFraction = 1.0 - (LeftInhaledAgentVolumeFraction + RightInhaledAgentVolumeFraction);
  
  //Check the O2 Source
  //Note: You're only allowed to use one at a time
  if (GetOxygenSource() == cdm::AnesthesiaMachineData_eOxygenSource_Wall)
  {
    if (m_actions->HasOxygenWallPortPressureLoss())
    {    
      m_O2InletVolumeFraction.SetValue(0.0);
    }
    else
    {
      m_O2InletVolumeFraction.SetValue(RemainingVolumeFraction * GetOxygenFraction().GetValue());
    }
  }
  else if (GetOxygenSource() == cdm::AnesthesiaMachineData_eOxygenSource_BottleOne || GetOxygenSource() == cdm::AnesthesiaMachineData_eOxygenSource_BottleTwo)
  {
    if (m_actions->HasOxygenTankPressureLoss())
    {
      m_O2InletVolumeFraction.SetValue(0.0);
    }
    else
    {
      m_O2InletVolumeFraction.SetValue(RemainingVolumeFraction * GetOxygenFraction().GetValue());
    }
  }
  
  //Check if the inlet bottle is empty
  if(GetOxygenSource() == cdm::AnesthesiaMachineData_eOxygenSource_BottleOne)  
  {
    if (GetOxygenBottleOne().GetVolume().GetValue(VolumeUnit::L) <= 0.0)
    {
      m_O2InletVolumeFraction.SetValue(0.0);
    }
  }
  else if(GetOxygenSource() == cdm::AnesthesiaMachineData_eOxygenSource_BottleTwo)
  {
    if (GetOxygenBottleTwo().GetVolume().GetValue(VolumeUnit::L) <= 0.0)
    {
      m_O2InletVolumeFraction.SetValue(0.0);
    }
  }
  
  double dO2VolumeFraction = 0.0;
  double dCO2VolumeFraction = 0.0;

  ///\todo: Do this stuff more generically by looping over all substances

  if (GetPrimaryGas() == cdm::AnesthesiaMachineData_ePrimaryGas_Air)
  {
    dO2VolumeFraction = RemainingVolumeFraction * m_ambientO2->GetVolumeFraction().GetValue();
    dCO2VolumeFraction = RemainingVolumeFraction * m_ambientCO2->GetVolumeFraction().GetValue();
  }
  //else Nitrogen, no additional O2 or CO2

  dO2VolumeFraction += m_O2InletVolumeFraction.GetValue();
  double dN2VolumeFraction = 1.0 - dO2VolumeFraction - dCO2VolumeFraction - LeftInhaledAgentVolumeFraction - RightInhaledAgentVolumeFraction;

  //Make sure we don't get a super small negative value
  dN2VolumeFraction = MAX(dN2VolumeFraction, 0.0);

  m_gasSourceO2->GetVolumeFraction().SetValue(dO2VolumeFraction);
  m_gasSourceCO2->GetVolumeFraction().SetValue(dCO2VolumeFraction);
  m_gasSourceN2->GetVolumeFraction().SetValue(dN2VolumeFraction);
  m_gasSource->Balance(BalanceGasBy::VolumeFraction);
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Checks the status of the oxygen source.
///
/// \details
/// Checks the status of the oxygen source. If an oxygen bottle is in use, ensures that the bottle still has oxygen. 
/// If the oxygen bottle no longer has oxygen, then an event is sent to the log notifying the user.
//--------------------------------------------------------------------------------------------------
void AnesthesiaMachine::CalculateSourceStatus()
{
  double dFlow_LPerS = 0.0;
  if (m_pGasSourceToGasInlet->HasFlow())
  {
    dFlow_LPerS = m_pGasSourceToGasInlet->GetFlow(VolumePerTimeUnit::L_Per_s);
  }

  if(GetOxygenSource() == cdm::AnesthesiaMachineData_eOxygenSource_BottleOne)
  {    
    double dBottle1Volume_L = GetOxygenBottleOne().GetVolume().GetValue(VolumeUnit::L);
    if (dBottle1Volume_L > 0.0)
    {
      dBottle1Volume_L -= m_dt_s * dFlow_LPerS * m_O2InletVolumeFraction.GetValue();
    }
    else if (dBottle1Volume_L <= 0.0) //Empty
    {
      /// \event %AnesthesiaMachine: Oxygen bottle 1 is exhausted. There is no longer any oxygen to provide via the anesthesia machine.
      SetEvent(cdm::AnesthesiaMachineData_eEvent_OxygenBottleOneExhausted, true, m_data.GetSimulationTime());
      dBottle1Volume_L = 0.0;
    }
    GetOxygenBottleOne().GetVolume().SetValue(dBottle1Volume_L, VolumeUnit::L);
  }
  else if(GetOxygenSource() == cdm::AnesthesiaMachineData_eOxygenSource_BottleTwo)
  {
    double dBottle2Volume_L = GetOxygenBottleTwo().GetVolume().GetValue(VolumeUnit::L);
    if (dBottle2Volume_L > 0.0)
    {
      dBottle2Volume_L -= m_dt_s * dFlow_LPerS * m_O2InletVolumeFraction.GetValue();
    }
    else if (dBottle2Volume_L <= 0.0)
    {
      /// \event %AnesthesiaMachine: Oxygen bottle 2 is exhausted. There is no longer any oxygen to provide via the anesthesia machine.
      SetEvent(cdm::AnesthesiaMachineData_eEvent_OxygenBottleTwoExhausted, true, m_data.GetSimulationTime());
      dBottle2Volume_L = 0.0;
    }
    GetOxygenBottleTwo().GetVolume().SetValue(dBottle2Volume_L, VolumeUnit::L);
  }
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Scales the resistance of anesthesia machine equipment in the presence of a leak.
///
/// \details
/// The resistance of the mask, tube, and y piece are scaled based on the presence and severity of a leak.
/// This will influence the circuit analyis and will effect the flow and gas mixture provided by the
/// anesthesia machine.
/// This occurs during preprocess. 
//--------------------------------------------------------------------------------------------------
void AnesthesiaMachine::CalculateEquipmentLeak()
{
  //Note: You should be able to stack failures, if you're so inclined
  
  if (m_data.GetAirwayMode() == pulse::eAirwayMode::Free)
  {
    return;
  }
  else if (m_data.GetAirwayMode() == pulse::eAirwayMode::AnesthesiaMachine)
  {
    if (m_Connection == cdm::AnesthesiaMachineData_eConnection_Tube)
    {
      if (m_actions->HasTubeCuffLeak() || m_actions->HasYPieceDisconnect())
      {
        double CuffLeakSeverity = 0.0;
        double YPieceDisconnectSeverity = 0.0;
        double TotalSeverity = 0.0;

        if (m_actions->HasTubeCuffLeak())
        {
          CuffLeakSeverity = m_actions->GetTubeCuffLeak()->GetSeverity().GetValue();
        }
        if (m_actions->HasYPieceDisconnect())
        {
          YPieceDisconnectSeverity = m_actions->GetYPieceDisconnect()->GetSeverity().GetValue();
        }

        //Combine the severities
        TotalSeverity = CuffLeakSeverity + YPieceDisconnectSeverity;
        if (TotalSeverity > 1.0)
        {
          TotalSeverity = 1.0;
        }

        double dResistance = GeneralMath::ResistanceFunction(10.0, m_dValveClosedResistance_cmH2O_s_Per_L, m_dValveOpenResistance_cmH2O_s_Per_L, TotalSeverity);
        m_pAnesthesiaConnectionToEnvironment->GetNextResistance().SetValue(dResistance, FlowResistanceUnit::cmH2O_s_Per_L);
      }
    }
  else if (m_Connection == cdm::AnesthesiaMachineData_eConnection_Mask)
  {
    if (m_actions->HasMaskLeak() || m_actions->HasYPieceDisconnect())
    {
      double MaskLeakSeverity = 0.0;
      double YPieceDisconnectSeverity = 0.0;
      double TotalSeverity = 0.0;

      if (m_actions->HasMaskLeak())
      {
        MaskLeakSeverity = m_actions->GetMaskLeak()->GetSeverity().GetValue();
      }
      if (m_actions->HasYPieceDisconnect())
      {
        YPieceDisconnectSeverity = m_actions->GetYPieceDisconnect()->GetSeverity().GetValue();
      }

      //Combine the severities
      TotalSeverity = MaskLeakSeverity + YPieceDisconnectSeverity;
      if (TotalSeverity > 1.0)
      {
        TotalSeverity = 1.0;
      }

      double dResistance = GeneralMath::ResistanceFunction(10.0, m_dValveClosedResistance_cmH2O_s_Per_L, m_dValveOpenResistance_cmH2O_s_Per_L, TotalSeverity);
      m_pAnesthesiaConnectionToEnvironment->GetNextResistance().SetValue(dResistance, FlowResistanceUnit::cmH2O_s_Per_L);
    }
  }
  }
  else
  {
    /// \error Error: Invalid Airway Mode Encountered in Compute Equipment Leak function
    Error("Invalid Airway Mode Encountered in Compute Equipment Leak function");
  }
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Valve resistance calculation
///
/// \details
/// The method checks for the breathing cycle and sets the inspiratory or expiratory valves as open or closed
/// by adjusting the corresponding valve resistances. The method also changes valve resistances to simulate equipment  
/// failures that correspond to valve leak and obstructions  
//--------------------------------------------------------------------------------------------------
void AnesthesiaMachine::CalculateValveResistances()
{  
  //Assume there's no leak or obstruction
  double dInspValveOpenResistance = m_dValveOpenResistance_cmH2O_s_Per_L;
  double dInspValveClosedResistance = m_pInspiratoryLimbToYPiece->GetNextResistance(FlowResistanceUnit::cmH2O_s_Per_L);
  double dExpValveOpenResistance = m_dValveOpenResistance_cmH2O_s_Per_L;
  double dExpValveClosedResistance = m_pYPieceToExpiratoryLimb->GetNextResistance(FlowResistanceUnit::cmH2O_s_Per_L);

  //Handle leaks and obstructions
  if (m_actions->HasInspiratoryValveLeak())
  {
    double severity = m_actions->GetInspiratoryValveLeak()->GetSeverity().GetValue();
    dInspValveOpenResistance = GeneralMath::ResistanceFunction(10.0, dInspValveClosedResistance, dInspValveOpenResistance, severity);
  }
  else if (m_actions->HasInspiratoryValveObstruction())
  {
    double severity = m_actions->GetInspiratoryValveObstruction()->GetSeverity().GetValue();
    dInspValveClosedResistance = GeneralMath::ResistanceFunction(10.0, dInspValveOpenResistance, dInspValveClosedResistance, severity);
  }

  if (m_actions->HasExpiratoryValveLeak())
  {
    double severity = m_actions->GetExpiratoryValveLeak()->GetSeverity().GetValue();
    dExpValveOpenResistance = GeneralMath::ResistanceFunction(10.0, dExpValveClosedResistance, dExpValveOpenResistance, severity);
  }
  else if (m_actions->HasExpiratoryValveObstruction())
  {
    double severity = m_actions->GetExpiratoryValveObstruction()->GetSeverity().GetValue();
    dExpValveClosedResistance = GeneralMath::ResistanceFunction(10.0, dExpValveOpenResistance, dExpValveClosedResistance, severity);
  }
  
  //Set the value based on where we are in the cycle
  if (m_inhaling)
  {
    m_pInspiratoryLimbToYPiece->GetNextResistance().SetValue(dInspValveClosedResistance, FlowResistanceUnit::cmH2O_s_Per_L);
    m_pYPieceToExpiratoryLimb->GetNextResistance().SetValue(dExpValveOpenResistance, FlowResistanceUnit::cmH2O_s_Per_L);
  }
  else
  {
    m_pInspiratoryLimbToYPiece->GetNextResistance().SetValue(dInspValveOpenResistance, FlowResistanceUnit::cmH2O_s_Per_L);
    m_pYPieceToExpiratoryLimb->GetNextResistance().SetValue(dExpValveClosedResistance, FlowResistanceUnit::cmH2O_s_Per_L);
  }
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculates ventilator pressure
///
/// \details
/// The Anesthesia machine employs pressure-control ventilation mode. This method calculates the 
/// control ventilator pressure that drives the gas flow in the breathing circle. During inspiration,
/// the ventilator pressure is set to pre-defined constant value to serve as an input pressure source.
/// This causes gas to flow into the inspiratory limb path. The pressure is dropped to much low pressure during the expiration 
/// phase to allow gas return to the ventilator.
//--------------------------------------------------------------------------------------------------
void AnesthesiaMachine::CalculateVentilator()
{
  //Calculate the driver pressure
  double dDriverPressure = 0.0;
  if (m_inhaling)
  {
    dDriverPressure = GetVentilatorPressure(PressureUnit::cmH2O);
  }
  else
  {
    dDriverPressure = GetPositiveEndExpiredPressure(PressureUnit::cmH2O);
  }  
  if (m_actions->HasVentilatorPressureLoss())
  {
    double severity = m_actions->GetVentilatorPressureLoss()->GetSeverity().GetValue();
    dDriverPressure *= (1 - severity);
  }
  m_pEnvironmentToVentilator->GetNextPressureSource().SetValue(dDriverPressure, PressureUnit::cmH2O);  
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Calculates respiration cycle
///
/// \details
/// The inspiratory and expiratory phase times are calculated based on
/// a pre-set respiration rate and inspiration-expiration ratio parameters. These parameters are selected as
/// input parameters for the anesthesia machine configurations.
//--------------------------------------------------------------------------------------------------
void AnesthesiaMachine::CalculateCyclePhase()
{
  //Determine where we are in the cycle
  m_currentbreathingCycleTime.IncrementValue(m_dt_s,TimeUnit::s);
  if (m_currentbreathingCycleTime.GetValue(TimeUnit::s) > m_totalBreathingCycleTime.GetValue(TimeUnit::s)) //End of the cycle
  {
    m_totalBreathingCycleTime.SetValue(0.0, TimeUnit::s);
    m_currentbreathingCycleTime.SetValue(0.0, TimeUnit::s);

    double dVentilationFrequency_PerMin = GetRespiratoryRate(FrequencyUnit::Per_min);
    if (dVentilationFrequency_PerMin > 0)
    {
      m_totalBreathingCycleTime.SetValue(60.0 / dVentilationFrequency_PerMin,TimeUnit::s); //Total time of one breathing cycle  
    }

    double IERatio = GetInspiratoryExpiratoryRatio().GetValue();
    m_inspirationTime.SetValue(IERatio * m_totalBreathingCycleTime.GetValue(TimeUnit::s) / (1.0 + IERatio),TimeUnit::s);
  }
  
  if (m_currentbreathingCycleTime.GetValue(TimeUnit::s) < m_inspirationTime.GetValue(TimeUnit::s)) //Inspiration
  {
    m_inhaling = true;
  }
  else //Expiration
  {
    m_inhaling = false;
  }
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Checks Relief Valve Pressure
///
/// \details
/// Assigns relief valve pressure as a pressure source based on the pressure setting and checks if the status 
/// of the relief valve is open or closed.
//--------------------------------------------------------------------------------------------------
void AnesthesiaMachine::CheckReliefValve()
{  
  //Set the Pressure Source based on the setting
  double dValvePressure_cmH2O = GetReliefValvePressure(PressureUnit::cmH2O);
  m_pEnvironmentToReliefValve->GetNextPressureSource().SetValue(dValvePressure_cmH2O, PressureUnit::cmH2O);

  //Check to see if it reached the pressure threshold  
  if (!IsEventActive(cdm::AnesthesiaMachineData_eEvent_ReliefValveActive) && m_pSelectorToReliefValve->GetNextValve() == cdm::eGate::Closed)
  {
    /// \event %AnesthesiaMachine: Relief Valve is active. The pressure setting has been exceeded.
    SetEvent(cdm::AnesthesiaMachineData_eEvent_ReliefValveActive, true, m_data.GetSimulationTime());
  }
  else if (IsEventActive(cdm::AnesthesiaMachineData_eEvent_ReliefValveActive) && m_pSelectorToReliefValve->GetNextValve() == cdm::eGate::Open)
  {
    SetEvent(cdm::AnesthesiaMachineData_eEvent_ReliefValveActive, false, m_data.GetSimulationTime());
  }

  //Always try to let it run without the relief valve operational (i.e. closed (i.e. allowing flow)), otherwise it will always stay shorted
  m_pSelectorToReliefValve->SetNextValve(cdm::eGate::Open);
}
