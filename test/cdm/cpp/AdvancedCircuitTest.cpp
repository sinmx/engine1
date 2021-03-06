/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

#include "CommonDataModelTest.h"
#include "utils/testing/SETestReport.h"
#include "utils/testing/SETestCase.h"
#include "utils/testing/SETestSuite.h"
#include "circuit/electrical/SEElectricalCircuit.h"
#include "circuit/electrical/SEElectricalCircuitNode.h"
#include "circuit/electrical/SEElectricalCircuitPath.h"
#include "circuit/electrical/SEElectricalCircuitCalculator.h"
#include "circuit/SECircuitManager.h"
#include "circuit/fluid/SEFluidCircuitCalculator.h"
#include "circuit/thermal/SEThermalCircuit.h"
#include "circuit/thermal/SEThermalCircuitNode.h"
#include "circuit/thermal/SEThermalCircuitPath.h"
#include "circuit/thermal/SEThermalCircuitCalculator.h"
#include "compartment/thermal/SEThermalCompartment.h"
#include "compartment/thermal/SEThermalCompartmentLink.h"
#include "properties/SEScalarPressure.h"
#include "properties/SEScalarFlowResistance.h"
#include "properties/SEScalarFlowCompliance.h"
#include "properties/SEScalarFlowInertance.h"
#include "properties/SEScalarTime.h"
#include "properties/SEScalarVolume.h"
#include "properties/SEScalarVolumePerTime.h"
#include "utils/DataTrack.h"

//*********************************************************
//Circuit Setup
//*********************************************************
/*
void InterCircuitTestSetup()
{
  //Circuit 1
  AdvancedCircuit1 = new SEFluidCircuit("Fluid",m_Logger);


  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& Node1 = fluidCircuit->CreateNode(1,"Node1");
  SEFluidCircuitNode& Node2 = fluidCircuit->CreateNode(2,"Node2");
  SEFluidCircuitNode& Node3 = fluidCircuit->CreateNode(3,"Node3");
  SEFluidCircuitNode& Node4 = fluidCircuit->CreateNode(4,"Node4");

  //Reference node
  //This works like ground - I added code to allow it to have a value other than 0
  fluidCircuit->AddReferenceNode(Node4);
  Node4.GetNextPressure().SetValue(0, PressureUnit::Pa);

  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& Path2 = fluidCircuit->CreatePath(Node1,Node2,2,"Path2");
  Path2.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path3 = fluidCircuit->CreatePath(Node2,Node3,3,"Path3");
  Path3.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path4 = fluidCircuit->CreatePath(Node3,Node4,4,"Path4");
  Path4.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);

  //Pressure source
  SEFluidCircuitPath& Path1 = fluidCircuit->CreatePath(Node4,Node1,1,"Path1");
  Path1.GetNextPressureSource().SetValue(20, PressureUnit::Pa);


  //Circuit 2
  AdvancedCircuit2 = new SEFluidCircuit(m_Logger);


  SEFluidCircuitNode& Node5 = AdvancedCircuit2->CreateNode(5,"Node5");
  SEFluidCircuitNode& Node6 = AdvancedCircuit2->CreateNode(6,"Node6");

  AdvancedCircuit2->AddReferenceNode(Node6);
  Node6.GetNextPressure().SetValue(0, PressureUnit::Pa);

  SEFluidCircuitPath& Path5 = AdvancedCircuit2->CreatePath(Node5,Node6,5,"Path5");
  Path5.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);


  //Connection
  Connection = new SEInterCircuitConnection(m_Logger);
  //TODO: Why doesn't it work reversed like this?
  //Connection->CreateInterCircuitConnection("Connection", AdvancedCircuit1, Node2, AdvancedCircuit2, Node5);
  Connection->CreateInterCircuitConnection("Connection", AdvancedCircuit2, Node5, AdvancedCircuit1, Node2);
}*/

//*********************************************************
//PreProcesses
//*********************************************************
void CommonDataModelTest::FluidPreProcess(SEFluidCircuit& c, double dT)
{
  double dPotential = 20 + 20 * sin(dT);
  c.GetPath("Path1")->GetNextPressureSource().SetValue(dPotential, PressureUnit::Pa);
}


//*********************************************************
//Main Calls
//*********************************************************

void CommonDataModelTest::ElectricalCircuitTest(const std::string& sTestDirectory)
{
  m_Logger->ResetLogFile(sTestDirectory + "/ElectricalCircuitTest.log");
  double timeStep_s = 1.0 / 165.0;
  Info("Electric Circuit");
  SEElectricalCircuit* ElectricalCircuit = &m_Circuits->CreateElectricalCircuit("Electric");
  SEElectricalCircuitCalculator ElectricalCircuitCalculator(m_Logger);
  ElectricalCircuit->StateChange();
  std::string sOutputFile = sTestDirectory + "/ElectricalCircuit.txt";
  //-----------------------------------------------------------
  //Nodes
  SEElectricalCircuitNode& Node1 = ElectricalCircuit->CreateNode("Node1");
  SEElectricalCircuitNode& Node2 = ElectricalCircuit->CreateNode("Node2");
  SEElectricalCircuitNode& Node3 = ElectricalCircuit->CreateNode("Node3");
  SEElectricalCircuitNode& Node4 = ElectricalCircuit->CreateNode("Node4");
  //Reference node
  ElectricalCircuit->AddReferenceNode(Node4);
  Node4.GetNextVoltage().SetValue(0, ElectricPotentialUnit::V);
  //-----------------------------------------------------------
  //Paths
  SEElectricalCircuitPath& Path2 = ElectricalCircuit->CreatePath(Node1, Node2, "Path2");
  Path2.GetNextResistance().SetValue(25, ElectricResistanceUnit::Ohm);
  SEElectricalCircuitPath& Path3 = ElectricalCircuit->CreatePath(Node2, Node3, "Path3");
  Path3.GetNextResistance().SetValue(25, ElectricResistanceUnit::Ohm);
  SEElectricalCircuitPath& Path4 = ElectricalCircuit->CreatePath(Node3, Node4, "Path4");
  Path4.GetNextResistance().SetValue(25, ElectricResistanceUnit::Ohm);
  //Pressure source
  SEElectricalCircuitPath& Path1 = ElectricalCircuit->CreatePath(Node4, Node1, "Path1");
  Path1.GetNextVoltageSource().SetValue(20, ElectricPotentialUnit::V);
  ElectricalCircuit->StateChange();

  // Lock it so you cannot modify Quantity/Potential/Flux values with out a source
  m_Circuits->SetReadOnly(true);

  DataTrack trk1;
  bool serialized = false;
  double sample = 0;
  double dT = 0.0;
  while (dT < 100)
  {
    //PreProcess - to be done by the systems
    ElectricalCircuit->GetPath("Path1")->GetNextVoltageSource().SetValue(20 + 20 * sin(dT), ElectricPotentialUnit::V);
    //Process
    ElectricalCircuitCalculator.Process(*ElectricalCircuit, timeStep_s);
    //PostProcess
    ElectricalCircuitCalculator.PostProcess(*ElectricalCircuit);
    dT += timeStep_s;
    //sampleDT += dDT;
    sample += timeStep_s;
    if (sample > 0.1)// every 0.1 seconds, track state of circuit
    {
      sample = 0;
      trk1.Track(dT, *ElectricalCircuit);
    }
    if (!serialized && dT > 80)
    {
      serialized = true;
      std::string pbaDir = sTestDirectory + "/ElectricalCircuit.pba";
      TestCircuitSerialization(pbaDir);      
      ElectricalCircuit = m_Circuits->GetElectricalCircuit("Electric");
    }
  }
  trk1.WriteTrackToFile(sOutputFile.c_str());
  m_Circuits->Clear();
}

void CommonDataModelTest::FluidCircuitTest(const std::string& sTestDirectory)
{
  m_Logger->ResetLogFile(sTestDirectory + "/FluidCircuitTest.log");
  double timeStep_s = 1.0 / 165.0;
  Info("Fluid Circuit");
  SEFluidCircuit*  fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");
  SEFluidCircuitCalculator      fluidCalculator(m_Logger);
  fluidCircuit->StateChange();
  std::string sOutputFile = sTestDirectory + "/FluidCircuit.txt";
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& Node1 = fluidCircuit->CreateNode("Node1");
  SEFluidCircuitNode& Node2 = fluidCircuit->CreateNode("Node2");
  SEFluidCircuitNode& Node3 = fluidCircuit->CreateNode("Node3");
  SEFluidCircuitNode& Node4 = fluidCircuit->CreateNode("Node4");
  //Reference node
  fluidCircuit->AddReferenceNode(Node4);
  Node4.GetNextPressure().SetValue(0, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& Path2 = fluidCircuit->CreatePath(Node1, Node2, "Path2");
  Path2.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path3 = fluidCircuit->CreatePath(Node2, Node3, "Path3");
  Path3.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path4 = fluidCircuit->CreatePath(Node3, Node4, "Path4");
  Path4.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  //Pressure source
  SEFluidCircuitPath& Path1 = fluidCircuit->CreatePath(Node4, Node1, "Path1");
  Path1.GetNextPressureSource().SetValue(20, PressureUnit::Pa);
  fluidCircuit->StateChange();

  // Lock it so you cannot modify Quantity/Potential/Flux values with out a source
  m_Circuits->SetReadOnly(true);

  DataTrack trk1;
  bool serialized = false;
  double sample = 0;
  double dT = 0.0;
  while (dT < 100)
  {
    //PreProcess - to be done by the systems
    fluidCircuit->GetPath("Path1")->GetNextPressureSource().SetValue(20 + 20 * sin(dT), PressureUnit::Pa);
    //Process
    fluidCalculator.Process(*fluidCircuit, timeStep_s);
    //PostProcess
    fluidCalculator.PostProcess(*fluidCircuit);
    dT += timeStep_s;
    //sampleDT += dDT;
    sample += timeStep_s;
    if (sample > 0.1)// every 0.1 seconds, track state of circuit
    {
      sample = 0;
      trk1.Track(dT, *fluidCircuit);
    }
    if (!serialized && dT > 80)
    {
      serialized = true;
      std::string pbaDir = sTestDirectory + "/FluidCircuit.pba";
      TestCircuitSerialization(pbaDir);
      fluidCircuit = m_Circuits->GetFluidCircuit("Fluid");
    }
  }
  trk1.WriteTrackToFile(sOutputFile.c_str());
  m_Circuits->Clear();
}

void CommonDataModelTest::ThermalCircuitTest(const std::string& sTestDirectory)
{
  m_Logger->ResetLogFile(sTestDirectory + "/ThermalCircuitTest.log");
  double timeStep_s = 1.0 / 165.0;
  Info("Thermal Circuit");
  SEThermalCircuit* ThermalCircuit = &m_Circuits->CreateThermalCircuit("Thermal");
  SEThermalCircuitCalculator    ThermalCircuitCalculator(m_Logger);
  ThermalCircuit->StateChange();
  std::string sOutputFile = sTestDirectory + "/ThermalCircuit.txt";
  //-----------------------------------------------------------
  //Nodes
  SEThermalCircuitNode& Node1 = ThermalCircuit->CreateNode("Node1");
  SEThermalCircuitNode& Node2 = ThermalCircuit->CreateNode("Node2");
  SEThermalCircuitNode& Node3 = ThermalCircuit->CreateNode("Node3");
  SEThermalCircuitNode& Node4 = ThermalCircuit->CreateNode("Node4");
  //Reference node
  ThermalCircuit->AddReferenceNode(Node4);
  Node4.GetNextTemperature().SetValue(0, TemperatureUnit::K);
  //-----------------------------------------------------------
  //Paths
  SEThermalCircuitPath& Path2 = ThermalCircuit->CreatePath(Node1, Node2, "Path2");
  Path2.GetNextResistance().SetValue(25, HeatResistanceUnit::K_Per_W);
  SEThermalCircuitPath& Path3 = ThermalCircuit->CreatePath(Node2, Node3, "Path3");
  Path3.GetNextResistance().SetValue(25, HeatResistanceUnit::K_Per_W);
  SEThermalCircuitPath& Path4 = ThermalCircuit->CreatePath(Node3, Node4, "Path4");
  Path4.GetNextResistance().SetValue(25, HeatResistanceUnit::K_Per_W);
  //Pressure source
  SEThermalCircuitPath& Path1 = ThermalCircuit->CreatePath(Node4, Node1, "Path1");
  Path1.GetNextTemperatureSource().SetValue(20, TemperatureUnit::K);
  ThermalCircuit->StateChange();

  // Lock it so you cannot modify Quantity/Potential/Flux values with out a source
  m_Circuits->SetReadOnly(true);

  DataTrack trk1;
  bool serialized = false;
  double sample = 0;
  double dT = 0.0;
  while (dT < 100)
  {
    //PreProcess - to be done by the systems
    ThermalCircuit->GetPath("Path1")->GetNextTemperatureSource().SetValue(20 + 20 * sin(dT), TemperatureUnit::K);
    //Process
    ThermalCircuitCalculator.Process(*ThermalCircuit, timeStep_s);
    //PostProcess
    ThermalCircuitCalculator.PostProcess(*ThermalCircuit);
    dT += timeStep_s;
    //sampleDT += dDT;
    sample += timeStep_s;
    if (sample > 0.1)// every 0.1 seconds, track state of circuit
    {
      sample = 0;
      trk1.Track(dT, *ThermalCircuit);
      /*for (SEThermalCircuitCompartmentLink* link : graph.GetLinks())
      {
        if (link->HasHeatTransferRate())
          trk1.Probe(link->GetName() + "Link_HeatTransferRate", dT, link->GetHeatTransferRate().GetValue(PowerUnit::W));
      }
      for (SEThermalCircuitCompartment* cmpt : graph.GetCompartments())
      {
        if(cmpt->HasTemperature())
          trk1.Probe(cmpt->GetName() + "Cmpt_Temperature", dT, cmpt->GetTemperature().GetValue(TemperatureUnit::K));
        if(cmpt->HasHeat())
          trk1.Probe(cmpt->GetName() + "Cmpt_Heat", dT, cmpt->GetHeat().GetValue(EnergyUnit::J));
      }*/
    }
    if (!serialized && dT > 80)
    {
      serialized = true;
      std::string pbaDir = sTestDirectory + "/ThermalCircuit.pba"; 
      TestCircuitSerialization(pbaDir);
      ThermalCircuit = m_Circuits->GetThermalCircuit("Thermal");
    }
  }
  trk1.WriteTrackToFile(sOutputFile.c_str());
  m_Circuits->Clear();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Tests the combined circuit methodology
///
/// \param  sTestDirectory points to the unit test directory
///
/// \details
/// This tests the functionality of combining separate circuits into once single circuit that can be
/// calculated, such as when the anesthesia machine connects to the respiratory system.
//--------------------------------------------------------------------------------------------------
void CommonDataModelTest::CombinedCircuitTest(const std::string& sTestDirectory)
{
  std::cout << "CombinedCircuitTest\n";
  m_Logger->ResetLogFile(sTestDirectory + "/CombinedCircuitTest.log");
  TimingProfile p;
  double timeStep_s = 1.0 / 165.0;
  double currentTime_s = 0.0;
  DataTrack trk1;
  SEFluidCircuitCalculator fluidCalculator(m_Logger);

  //Master Circuit
  SEFluidCircuit* MasterCircuit = &m_Circuits->CreateFluidCircuit("Master");
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& MasterNode1 = MasterCircuit->CreateNode("MasterNode1");
  SEFluidCircuitNode& MasterNode2 = MasterCircuit->CreateNode("MasterNode2");
  SEFluidCircuitNode& MasterNode3 = MasterCircuit->CreateNode("MasterNode3");
  SEFluidCircuitNode& MasterNode4 = MasterCircuit->CreateNode("MasterNode4");
  //Reference node
  //This works like ground - I added code to allow it to have a value other than 0
  MasterCircuit->AddReferenceNode(MasterNode4);
  MasterNode4.GetNextPressure().SetValue(0, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& MasterPath2 = MasterCircuit->CreatePath(MasterNode1, MasterNode2, "MasterPath2");
  MasterPath2.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& MasterPath3 = MasterCircuit->CreatePath(MasterNode2, MasterNode3, "MasterPath3");
  MasterPath3.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& MasterPath4 = MasterCircuit->CreatePath(MasterNode3, MasterNode4, "MasterPath4");
  MasterPath4.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  //Pressure source
  SEFluidCircuitPath& MasterPath1 = MasterCircuit->CreatePath(MasterNode4, MasterNode1, "MasterPath1");
  MasterPath1.GetNextPressureSource().SetValue(20, PressureUnit::Pa);
  //Slave Circuit
   SEFluidCircuit* SlaveCircuit = &m_Circuits->CreateFluidCircuit("Slave");
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& SlaveNode1 = SlaveCircuit->CreateNode("SlaveNode1");
  SEFluidCircuitNode& SlaveNode2 = SlaveCircuit->CreateNode("SlaveNode2");
  SEFluidCircuitNode& SlaveNode3 = SlaveCircuit->CreateNode("SlaveNode3");
  SEFluidCircuitNode& SlaveNode4 = SlaveCircuit->CreateNode("SlaveNode4");
  //Reference node
  //This works like ground - I added code to allow it to have a value other than 0
  SlaveCircuit->AddReferenceNode(SlaveNode4);
  SlaveNode4.GetNextPressure().SetValue(0, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& SlavePath2 = SlaveCircuit->CreatePath(SlaveNode1, SlaveNode2, "SlavePath2");
  SlavePath2.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& SlavePath3 = SlaveCircuit->CreatePath(SlaveNode2, SlaveNode3, "SlavePath3");
  SlavePath3.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& SlavePath4 = SlaveCircuit->CreatePath(SlaveNode3, SlaveNode4, "SlavePath4");
  SlavePath4.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  //Pressure source
  SEFluidCircuitPath& SlavePath1 = SlaveCircuit->CreatePath(SlaveNode4, SlaveNode1, "SlavePath1");
  SlavePath1.GetNextPressureSource().SetValue(20, PressureUnit::Pa);
  //Create the combined circuit
  SEFluidCircuit* CombinedCircuit = &m_Circuits->CreateFluidCircuit("Combined");
  CombinedCircuit->AddCircuit(*MasterCircuit);
  CombinedCircuit->AddCircuit(*SlaveCircuit);
  SEFluidCircuitPath& GroundPath = CombinedCircuit->CreatePath(MasterNode4, SlaveNode4, "GroundPath");
  SEFluidCircuitPath& CombinedPath = CombinedCircuit->CreatePath(MasterNode3, SlaveNode3, "CombinedPath");
  CombinedCircuit->RemovePath(SlavePath1);
  CombinedCircuit->StateChange();

  bool serialized = false;
  double sample = 0;
  while (currentTime_s < 100)
  {
    //PreProcess - to be done by the systems
    double dPotential = 20 + 20 * sin(currentTime_s);
    MasterCircuit->GetPath("MasterPath1")->GetNextPressureSource().SetValue(dPotential, PressureUnit::Pa);
    //Process
    fluidCalculator.Process(*CombinedCircuit, timeStep_s);
    //PostProcess
    fluidCalculator.PostProcess(*CombinedCircuit);
    currentTime_s += timeStep_s;
    //sampleDT += dDT;
    sample += timeStep_s;
    if (sample > 0.1)// every 0.1 seconds, track state of circuit
    {
      sample = 0;
      trk1.Track(currentTime_s, *CombinedCircuit);
    }
    if (!serialized && currentTime_s > 80)
    {
      serialized = true;
      std::string pbaDir = sTestDirectory + "/CombinedCircuitTest.pba"; 
      TestCircuitSerialization(pbaDir);
      // Loading will create a new circuit, so we need to delete our old ones
      // And hook up to the new ones
      MasterCircuit = m_Circuits->GetFluidCircuit("Master");
      SlaveCircuit = m_Circuits->GetFluidCircuit("Slave");
      CombinedCircuit = m_Circuits->GetFluidCircuit("Combined");
    }
  }
  std::string sOutputFile = sTestDirectory + "/CombinedCircuit.txt";
  trk1.WriteTrackToFile(sOutputFile.c_str());
  m_Circuits->Clear();
}

void CommonDataModelTest::InterCircuitComparisonTest(const std::string& sTestDirectory)
{
  std::cout << "InterComparisonCircuitTest\n";

  m_Logger->ResetLogFile(sTestDirectory + "/InterCircuitComparisonTest.log");
  TimingProfile p;
  double timeStep_s = 1.0 / 165.0;
  double currentTime_s = 0.0;
  DataTrack trk1;
  DataTrack trk2;
  SEFluidCircuitCalculator fluidCalculator(m_Logger);

  //Test Circuit
  SEFluidCircuit* fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& Node1 = fluidCircuit->CreateNode("Node1");
  SEFluidCircuitNode& Node2 = fluidCircuit->CreateNode("Node2");
  SEFluidCircuitNode& Node3 = fluidCircuit->CreateNode("Node3");
  SEFluidCircuitNode& Node4 = fluidCircuit->CreateNode("Node4");
  //Reference node
  //This works like ground - I added code to allow it to have a value other than 0
  fluidCircuit->AddReferenceNode(Node4);
  Node4.GetNextPressure().SetValue(0, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& Path2 = fluidCircuit->CreatePath(Node1, Node2, "Path2");
  Path2.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path3 = fluidCircuit->CreatePath(Node2, Node3, "Path3");
  Path3.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path4 = fluidCircuit->CreatePath(Node3, Node4, "Path4");
  Path4.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path5 = fluidCircuit->CreatePath(Node2, Node4, "Path5");
  Path5.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  //Pressure source
  SEFluidCircuitPath& Path1 = fluidCircuit->CreatePath(Node4, Node1, "Path1");
  Path1.GetNextPressureSource().SetValue(20, PressureUnit::Pa);
  fluidCircuit->StateChange();

  bool serialized = false;
  double sample = 0;
  while (currentTime_s < 100)
  {
    //PreProcess - to be done by the systems
    FluidPreProcess(*fluidCircuit, currentTime_s);
    //Process
    fluidCalculator.Process(*fluidCircuit, timeStep_s);
    //PostProcess
    fluidCalculator.PostProcess(*fluidCircuit);
    currentTime_s += timeStep_s;
    sample += timeStep_s;
    if (sample > 0.1)// every 0.1 seconds, track state of circuit
    {
      sample = 0;
      trk1.Track(currentTime_s, *fluidCircuit);
    }
    if (!serialized && currentTime_s > 80)
    {
      serialized = true;
      std::string pbaDir = sTestDirectory + "/InterCircuitComparison.pba";
      TestCircuitSerialization(pbaDir);
      fluidCircuit = m_Circuits->GetFluidCircuit("Fluid");
    }
  }
  std::string sOutputFile = sTestDirectory + "/InterCircuitComparison.txt";
  trk1.WriteTrackToFile(sOutputFile.c_str());
  m_Circuits->Clear();
}

void CommonDataModelTest::InterCircuitIndividualTest(const std::string& sTestDirectory)
{
  std::cout << "InterCircuitIndividualTest\n";

  m_Logger->ResetLogFile(sTestDirectory + "/InterCircuitIndividualTest.log");
  TimingProfile p;
  double timeStep_s = 1.0 / 165.0;
  double currentTime_s = 0.0;
  DataTrack trk1;
  DataTrack trk2;
  SEFluidCircuitCalculator fluidCalculator(m_Logger);

  //Circuit 1
  SEFluidCircuit* fluidCircuit1 = &m_Circuits->CreateFluidCircuit("Fluid1");
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& Node1 = fluidCircuit1->CreateNode("Node1");
  SEFluidCircuitNode& Node2 = fluidCircuit1->CreateNode("Node2");
  SEFluidCircuitNode& Node3 = fluidCircuit1->CreateNode("Node3");
  SEFluidCircuitNode& Node4 = fluidCircuit1->CreateNode("Node4");
  //Reference node
  //This works like ground - I added code to allow it to have a value other than 0
  fluidCircuit1->AddReferenceNode(Node4);
  Node4.GetNextPressure().SetValue(0, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& Path2 = fluidCircuit1->CreatePath(Node1, Node2, "Path2");
  Path2.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path3 = fluidCircuit1->CreatePath(Node2, Node3, "Path3");
  Path3.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path4 = fluidCircuit1->CreatePath(Node3, Node4, "Path4");
  Path4.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  //Pressure source
  SEFluidCircuitPath& Path1 = fluidCircuit1->CreatePath(Node4, Node1, "Path1");
  Path1.GetNextPressureSource().SetValue(20, PressureUnit::Pa);
  //IC Pressure source
  SEFluidCircuitPath& Path5 = fluidCircuit1->CreatePath(Node4, Node2, "Path5");
  Path5.GetNextPressureSource().SetValue(20, PressureUnit::Pa);
  fluidCircuit1->StateChange();
  //Circuit 2
  SEFluidCircuit* fluidCircuit2 = &m_Circuits->CreateFluidCircuit("Fluid2");
  SEFluidCircuitNode& Node5 = fluidCircuit2->CreateNode("Node5");
  SEFluidCircuitNode& Node6 = fluidCircuit2->CreateNode("Node6");
  fluidCircuit2->AddReferenceNode(Node6);
  Node6.GetNextPressure().SetValue(0, PressureUnit::Pa);
  SEFluidCircuitPath& Path6 = fluidCircuit2->CreatePath(Node5, Node6, "Path6");
  Path6.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  //IC Flow source
  SEFluidCircuitPath& Path7 = fluidCircuit2->CreatePath(Node6, Node5, "Path7");
  Path7.GetNextFlowSource().SetValue(10, VolumePerTimeUnit::m3_Per_s);
  fluidCircuit2->StateChange();

  bool serialized = false;
  double sample = 0;
  while (currentTime_s < 100)
  {
    //Process
    fluidCalculator.Process(*fluidCircuit1, timeStep_s);
    fluidCalculator.Process(*fluidCircuit2, timeStep_s);
    //PostProcess
    fluidCalculator.PostProcess(*fluidCircuit1);
    fluidCalculator.PostProcess(*fluidCircuit2);

    currentTime_s += timeStep_s;
    sample += timeStep_s;
    if (sample > 0.1)// every 0.1 seconds, track state of circuit
    {
      sample = 0;
      trk1.Track(currentTime_s, *fluidCircuit1);
      trk2.Track(currentTime_s, *fluidCircuit2);
    }
    if (!serialized && currentTime_s > 80)
    {
      serialized = true;
      std::string pbaDir = sTestDirectory + "/InterCircuitIndividual.pba";
      TestCircuitSerialization(pbaDir);
      fluidCircuit1 = m_Circuits->GetFluidCircuit("Fluid1");
      fluidCircuit2 = m_Circuits->GetFluidCircuit("Fluid2");
    }
  }

  std::string sOutputFile = sTestDirectory + "/InterCircuitIndividual1.txt";
  std::string sOutputFile2 = sTestDirectory + "/InterCircuitIndividual2.txt";
  trk1.WriteTrackToFile(sOutputFile.c_str());
  trk2.WriteTrackToFile(sOutputFile2.c_str());
  m_Circuits->Clear();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Tests the interconnected circuit methodology
///
/// \param  sTestDirectory points to the unit test directory
///
/// \details
/// This tests the functionality of combining separate circuits into once single circuit that can be
/// calculated, such as when the anesthesia machine connects to the respiratory system.
//--------------------------------------------------------------------------------------------------
/*
void InterCircuitTest(const std::string& sTestDirectory)
{
  std::cout << "InterCircuitTest\n";

  m_Logger->ResetLogFile(sTestDirectory + "/InterCircuitTest.log");
  TimingProfile p;
  double dDT = 1.0/165.0;
  double dT = 0.0;
  DataTrack trk1;
  DataTrack trk2;
  SEFluidCircuitCalculator fluidCalculator(m_Logger);
  AdvancedCircuitCalculator.GetTimeStep().SetValue(dDT, TimeUnit::s);

  InterCircuitTestSetup();

  fluidCircuit->SetBaselinesFromNext();
  fluidCircuit->StateChange();

  AdvancedCircuit2->SetBaselinesFromNext();
  AdvancedCircuit2->StateChange();

  //p.Start("Sample");
  //double sampleDT=0;
  double sample = 0;
  while(dT < 100)
  {
    //PreProcess - to be done by the systems
    GenericPreProcess(AdvancedCircuit1, dT);
    Connection->PreProcess();

    //Process
    fluidCalculator.Process(AdvancedCircuit1);
    fluidCalculator.Process(AdvancedCircuit2);
    Connection->Process();

    //PostProcess
    fluidCalculator.PostProcess(AdvancedCircuit1);
    fluidCalculator.PostProcess(AdvancedCircuit2);
    Connection->PostProcess();

    dT += dDT;
    //sampleDT += dDT;

    sample+=dDT;
    if(sample>0.1)// every 0.1 seconds, track state of circuit
    {
      sample=0;
      trk1.Track(dT,fluidCircuit);
      trk2.Track(dT,*AdvancedCircuit2);
    }
  }

  std::string sOutputFile = sTestDirectory + "/InterCircuit.txt";
  std::string sOutputFile2 = sTestDirectory + "/InterCircuit2.txt";
  trk1.WriteTrackToFile(sOutputFile.c_str());
  trk2.WriteTrackToFile(sOutputFile2.c_str());

  delete AdvancedCircuit1;
  delete AdvancedCircuit2;
  delete Connection;
  delete AdvancedCircuitCalculator;
  
}
*/
//--------------------------------------------------------------------------------------------------
/// \brief
/// Tests the circuit errors
///
/// \param  sTestDirectory points to the unit test directory
///
/// \details
/// This tests the errors thrown when the circuit state is attempted to be calculated,
/// but contains something improperly setup.
//--------------------------------------------------------------------------------------------------
void CommonDataModelTest::CircuitErrorTest(const std::string& sTestDirectory)
{
  std::cout << "CircuitErrorTest\n";
  //Setup a basic circuit
  m_Logger->ResetLogFile(sTestDirectory + "/CombinedCircuitTest.log");
  double timeStep_s = 1.0 / 165.0;
  SEFluidCircuitCalculator fluidCalculator(m_Logger);
  SEFluidCircuit* fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& Node1 = fluidCircuit->CreateNode("Node1");
  SEFluidCircuitNode& Node2 = fluidCircuit->CreateNode("Node2");
  SEFluidCircuitNode& Node3 = fluidCircuit->CreateNode("Node3");
  SEFluidCircuitNode& Node4 = fluidCircuit->CreateNode("Node4");

  //Reference node
  //This works like ground - I added code to allow it to have a value other than 0
  fluidCircuit->AddReferenceNode(Node4);
  Node4.GetNextPressure().SetValue(0, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& Path2 = fluidCircuit->CreatePath(Node1, Node2, "Path2");
  Path2.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path3 = fluidCircuit->CreatePath(Node2, Node3, "Path3");
  Path3.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path4 = fluidCircuit->CreatePath(Node3, Node4, "Path4");
  Path4.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path5 = fluidCircuit->CreatePath(Node2, Node4, "Path5");
  Path5.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);

  //Pressure source
  SEFluidCircuitPath& Path1 = fluidCircuit->CreatePath(Node4, Node1, "Path1");
  Path1.GetNextPressureSource().SetValue(20, PressureUnit::Pa);

  //*****************************
  //Circuit Settings Failures
  //*****************************

  // None, as of yet, moving to templates enforced a lot of stuff
  Path2.GetNextCompliance().SetValue(25, FlowComplianceUnit::m3_Per_Pa);
  Path2.HasValidElements();
  m_Circuits->Clear();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Tests to ensure circuits can be changed dynamically
///
/// \param  sTestDirectory points to the unit test directory
///
/// \details
/// This tests the dynamic circuit methodology by changing path elements, as well as adding a path
/// in the middle of a run.
//--------------------------------------------------------------------------------------------------
void CommonDataModelTest::DynamicallyChangingCircuitTest(const std::string& sTestDirectory)
{
  std::cout << "DynamicallyChangingCircuitTest\n";
  m_Logger->ResetLogFile(sTestDirectory + "/DynamicallyChangingCircuitTest.log");
  TimingProfile p;
  double timeStep_s = 1.0 / 165.0;
  DataTrack trk1;
  SEFluidCircuitCalculator fluidCalculator(m_Logger);

  //Test Circuit
  SEFluidCircuit* fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& Node1 = fluidCircuit->CreateNode("Node1");
  SEFluidCircuitNode& Node2 = fluidCircuit->CreateNode("Node2");
  SEFluidCircuitNode& Node3 = fluidCircuit->CreateNode("Node3");
  SEFluidCircuitNode& Node4 = fluidCircuit->CreateNode("Node4");
  //Reference node
  fluidCircuit->AddReferenceNode(Node4);
  Node4.GetNextPressure().SetValue(0, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& Path2 = fluidCircuit->CreatePath(Node1, Node2, "Path2");
  Path2.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path3 = fluidCircuit->CreatePath(Node2, Node3, "Path3");
  Path3.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path4 = fluidCircuit->CreatePath(Node3, Node4, "Path4");
  Path4.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  //Pressure source
  SEFluidCircuitPath& Path1 = fluidCircuit->CreatePath(Node4, Node1, "Path1");
  Path1.GetNextPressureSource().SetValue(20, PressureUnit::Pa);
  fluidCircuit->StateChange();

  bool serialized = false;
  double sample = 0;
  double dT = 0.0;
  bool bChanged = false;
  while (dT < 100)
  {
    if (!bChanged && dT > 50)
    {
      std::cout << "Modify Circuit\n";
      //Change element types
      fluidCircuit->GetPath("Path2")->GetResistance().Invalidate();
      fluidCircuit->GetPath("Path2")->GetNextResistance().Invalidate();
      fluidCircuit->GetPath("Path2")->GetResistanceBaseline().Invalidate();
      fluidCircuit->GetPath("Path2")->GetNextCompliance().SetValue(0.02, FlowComplianceUnit::m3_Per_Pa);
      fluidCircuit->GetPath("Path2")->GetSourceNode().GetNextVolume().SetValue(10, VolumeUnit::m3);

      fluidCircuit->GetPath("Path3")->GetResistance().Invalidate();
      fluidCircuit->GetPath("Path3")->GetNextResistance().Invalidate();
      fluidCircuit->GetPath("Path3")->GetResistanceBaseline().Invalidate();
      fluidCircuit->GetPath("Path3")->GetNextInertance().SetValue(0.01, FlowInertanceUnit::Pa_s2_Per_m3);

      //Change the value, but leave the element the same
      fluidCircuit->GetPath("Path4")->GetNextResistance().SetValue(50, FlowResistanceUnit::Pa_s_Per_m3);
      fluidCircuit->GetPath("Path4")->GetResistanceBaseline().SetValue(50, FlowResistanceUnit::Pa_s_Per_m3);

      //Add a new Path
      SEFluidCircuitNode* Node2 = fluidCircuit->GetNode("Node2");
      SEFluidCircuitNode* Node4 = fluidCircuit->GetNode("Node4");
      SEFluidCircuitPath& Path5 = fluidCircuit->CreatePath(*Node2, *Node4, "Path5");
      Path5.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
      //Reset the baselines
      fluidCircuit->StateChange();
      bChanged = true;
    }
    //PreProcess - to be done by the systems
    FluidPreProcess(*fluidCircuit, dT);
    //Process
    fluidCalculator.Process(*fluidCircuit, timeStep_s);
    //PostProcess
    fluidCalculator.PostProcess(*fluidCircuit);
    dT += timeStep_s;
    sample += timeStep_s;
    if (sample > 0.1)// every 0.1 seconds, track state of circuit
    {
      sample = 0;
      trk1.Track(dT, *fluidCircuit);
    }
    if (!serialized && dT > 80)
    {
      serialized = true;
      std::string pbaDir = sTestDirectory + "/DynamicallyChangingCircuit.pba";
      TestCircuitSerialization(pbaDir);
      fluidCircuit = m_Circuits->GetFluidCircuit("Fluid");
    }
  }
  std::string sOutputFile = sTestDirectory + "/DynamicallyChangingCircuit.txt";
  trk1.WriteTrackToFile(sOutputFile.c_str());
  m_Circuits->Clear();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Tests the non-zero reference circuit methodology
///
/// \param  sTestDirectory points to the unit test directory
///
/// \details
/// This tests the ability to solve a circuit correctly with a positive and negative reference 
/// potential value.
//--------------------------------------------------------------------------------------------------
void CommonDataModelTest::NonZeroReferencePositive(const std::string& sTestDirectory)
{
  std::cout << "NonZeroReferencePositive\n";
  m_Logger->ResetLogFile(sTestDirectory + "/NonZeroReferencePositive.log");
  TimingProfile p;
  double timeStep_s = 1.0 / 165.0;
  DataTrack trk1;
  SEFluidCircuitCalculator fluidCalculator(m_Logger);
  SEFluidCircuit* fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& Node1 = fluidCircuit->CreateNode("Node1");
  SEFluidCircuitNode& Node2 = fluidCircuit->CreateNode("Node2");
  SEFluidCircuitNode& Node3 = fluidCircuit->CreateNode("Node3");
  SEFluidCircuitNode& Node4 = fluidCircuit->CreateNode("Node4");
  //Reference node
  //This works like ground - I added code to allow it to have a value other than 0
  fluidCircuit->AddReferenceNode(Node4);
  double dRef = 10;
  std::cout << "Ref = 10\n";
  Node4.GetNextPressure().SetValue(dRef, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& Path2 = fluidCircuit->CreatePath(Node1, Node2, "Path2");
  Path2.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path3 = fluidCircuit->CreatePath(Node2, Node3, "Path3");
  Path3.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path4 = fluidCircuit->CreatePath(Node3, Node4, "Path4");
  Path4.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  //Pressure source
  SEFluidCircuitPath& Path1 = fluidCircuit->CreatePath(Node4, Node1, "Path1");
  Path1.GetNextPressureSource().SetValue(20, PressureUnit::Pa);
  fluidCircuit->StateChange();

  bool serialized = false;
  double sample = 0;
  double dT = 0.0;
  while (dT < 100)
  {
    //PreProcess - to be done by the systems
    FluidPreProcess(*fluidCircuit, dT);
    //Process
    fluidCalculator.Process(*fluidCircuit, timeStep_s);
    //PostProcess
    fluidCalculator.PostProcess(*fluidCircuit);
    dT += timeStep_s;
    sample += timeStep_s;
    if (sample > 0.1)// every 0.1 seconds, track state of circuit
    {
      sample = 0;
      trk1.Track(dT, *fluidCircuit);
    }
    if (!serialized && dT > 80)
    {
      serialized = true;
      std::string pbaDir = sTestDirectory + "/NonZeroReferencePositive.pba";
      TestCircuitSerialization(pbaDir);
      fluidCircuit = m_Circuits->GetFluidCircuit("Fluid");
    }
  }
  std::string sOutputFile = sTestDirectory + "/NonZeroReferencePositive.txt";
  trk1.WriteTrackToFile(sOutputFile.c_str());
  m_Circuits->Clear();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Tests the non-zero reference circuit methodology
///
/// \param  sTestDirectory points to the unit test directory
///
/// \details
/// This tests the ability to solve a circuit correctly with a negative reference potential value.
//--------------------------------------------------------------------------------------------------
void CommonDataModelTest::NonZeroReferenceNegative(const std::string& sTestDirectory)
{
  std::cout << "NonZeroReferenceNegative\n";
  m_Logger->ResetLogFile(sTestDirectory + "/NonZeroReferenceNegative.log");
  TimingProfile p;
  double timeStep_s = 1.0 / 165.0;
  DataTrack trk1;
  SEFluidCircuitCalculator fluidCalculator(m_Logger);
  SEFluidCircuit* fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& Node1 = fluidCircuit->CreateNode("Node1");
  SEFluidCircuitNode& Node2 = fluidCircuit->CreateNode("Node2");
  SEFluidCircuitNode& Node3 = fluidCircuit->CreateNode("Node3");
  SEFluidCircuitNode& Node4 = fluidCircuit->CreateNode("Node4");
  //Reference node
  //This works like ground - I added code to allow it to have a value other than 0
  fluidCircuit->AddReferenceNode(Node4);
  double dRef = -10;
  std::cout << "Ref = -10\n";
  Node4.GetNextPressure().SetValue(dRef, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& Path2 = fluidCircuit->CreatePath(Node1, Node2, "Path2");
  Path2.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path3 = fluidCircuit->CreatePath(Node2, Node3, "Path3");
  Path3.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path4 = fluidCircuit->CreatePath(Node3, Node4, "Path4");
  Path4.GetNextResistance().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  //Pressure source
  SEFluidCircuitPath& Path1 = fluidCircuit->CreatePath(Node4, Node1, "Path1");
  Path1.GetNextPressureSource().SetValue(20, PressureUnit::Pa);
  fluidCircuit->StateChange();
  
  bool serialized = false;
  double sample = 0;
  double dT = 0.0;
  while (dT < 100)
  {
    //PreProcess - to be done by the systems
    FluidPreProcess(*fluidCircuit, dT);
    //Process
    fluidCalculator.Process(*fluidCircuit, timeStep_s);
    //PostProcess
    fluidCalculator.PostProcess(*fluidCircuit);
    dT += timeStep_s;
    sample += timeStep_s;
    if (sample > 0.1)// every 0.1 seconds, track state of circuit
    {
      sample = 0;
      trk1.Track(dT, *fluidCircuit);
    }
    if (!serialized && dT > 80)
    {
      serialized = true;
      std::string pbaDir = sTestDirectory + "/NonZeroReferenceNegative.pba";
      TestCircuitSerialization(pbaDir);
      fluidCircuit = m_Circuits->GetFluidCircuit("Fluid");
    }
  }
  std::string sOutputFile = sTestDirectory + "/NonZeroReferenceNegative.txt";
  trk1.WriteTrackToFile(sOutputFile.c_str());
  m_Circuits->Clear();
}

//--------------------------------------------------------------------------------------------------
/// \brief
/// Tests the polarized element methodology
///
/// \param  sTestDirectory points to the unit test directory
///
/// \details
/// This tests the functionality of a polarized capacitor and ensures that it functions normally
/// when the Source Pressure is greater than the Target Pressure, and functions as an open switch
/// when the Target Pressure is greater than the Source Pressure.
//--------------------------------------------------------------------------------------------------
void CommonDataModelTest::PolarizedCapacitorTest(const std::string& sTestDirectory)
{
  std::cout << "PolarizedCapacitorTest\n";
  m_Logger->ResetLogFile(sTestDirectory + "/PolarizedCapacitorTest.log");
  TimingProfile p;
  double timeStep_s = 1.0 / 100.0;
  double currentTime_s = 0.0;
  DataTrack trk1;
  SEFluidCircuitCalculator fluidCalculator(m_Logger);
  SEFluidCircuit* fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& Node1 = fluidCircuit->CreateNode("Node1");
  SEFluidCircuitNode& Node2 = fluidCircuit->CreateNode("Node2");
  SEFluidCircuitNode& Node3 = fluidCircuit->CreateNode("Node3");
  SEFluidCircuitNode& Node4 = fluidCircuit->CreateNode("Node4");
  //Reference node
  fluidCircuit->AddReferenceNode(Node4);
  Node4.GetNextPressure().SetValue(0, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& Path2 = fluidCircuit->CreatePath(Node2, Node1, "Path2");
  Path2.GetNextCompliance().SetValue(1, FlowComplianceUnit::m3_Per_Pa);
  Path2.SetNextPolarizedState(cdm::eGate::Closed);
  Path2.GetSourceNode().GetNextVolume().SetValue(0, VolumeUnit::m3);
  SEFluidCircuitPath& Path3 = fluidCircuit->CreatePath(Node2, Node3, "Path3");
  Path3.GetNextResistance().SetValue(1, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& Path4 = fluidCircuit->CreatePath(Node4, Node3, "Path4");
  Path4.GetNextPressureSource().SetValue(20, PressureUnit::Pa);
  SEFluidCircuitPath& Path1 = fluidCircuit->CreatePath(Node4, Node1, "Path1");
  Path1.GetNextPressureSource().SetValue(0, PressureUnit::Pa);
  fluidCircuit->StateChange();

  bool serialized = false;
  double sample = 0;
  while (currentTime_s < 150)
  {
    if (currentTime_s > 100)
    {
      //Change it back, but more pressure
      fluidCircuit->GetPath("Path4")->GetNextPressureSource().SetValue(30, PressureUnit::Pa);
      fluidCircuit->GetPath("Path1")->GetNextPressureSource().SetValue(0, PressureUnit::Pa);
    }
    else if (currentTime_s > 50)
    {
      //Change element types
      fluidCircuit->GetPath("Path1")->GetNextPressureSource().SetValue(20, PressureUnit::Pa);
      fluidCircuit->GetPath("Path4")->GetNextPressureSource().SetValue(0, PressureUnit::Pa);
    }
    //Process
    fluidCalculator.Process(*fluidCircuit, timeStep_s);
    //PostProcess
    fluidCalculator.PostProcess(*fluidCircuit);
    currentTime_s += timeStep_s;
    trk1.Track(currentTime_s, *fluidCircuit);
    if (!serialized && currentTime_s > 130)
    {
      serialized = true;
      std::string pbaDir = sTestDirectory + "/NonZeroReferenceNegative.pba";
      TestCircuitSerialization(pbaDir);
      fluidCircuit = m_Circuits->GetFluidCircuit("Fluid");
    }
  }
  std::string sOutputFile = sTestDirectory + "/PolarizedCapacitor.txt";
  trk1.WriteTrackToFile(sOutputFile.c_str());
  m_Circuits->Clear();
}

void CommonDataModelTest::PreChargeComplianceZeroVolume(const std::string& sTestDirectory)
{
  std::cout << "PreChargeComplianceZeroVolume\n";
  m_Logger->ResetLogFile(sTestDirectory + "/PreChargeComplianceZeroVolume.log");
  TimingProfile p;
  double timeStep_s = 1.0 / 165.0;
  double currentTime_s = 0.0;
  DataTrack trk1;

  SEFluidCircuitCalculator fluidCalculator(m_Logger);
  SEFluidCircuit* fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");

  SEFluidCircuitNode& node1 = fluidCircuit->CreateNode("node1");
  node1.GetPressure().SetValue(10.0, PressureUnit::mmHg);
  SEFluidCircuitNode& ground = fluidCircuit->CreateNode("node2");
  fluidCircuit->AddReferenceNode(ground);
  ground.GetNextPressure().SetValue(0.0, PressureUnit::mmHg);
  SEFluidCircuitPath& groundTonode1 = fluidCircuit->CreatePath(ground, node1, "groundTonode1");
  groundTonode1.GetPressureSourceBaseline().SetValue(10.0, PressureUnit::mmHg);
  SEFluidCircuitPath& node1Toground = fluidCircuit->CreatePath(node1, ground, "node1Toground");
  node1Toground.GetComplianceBaseline().SetValue(1.0, FlowComplianceUnit::mL_Per_mmHg);
  node1Toground.GetSourceNode().GetVolumeBaseline().SetValue(0.0, VolumeUnit::mL);
  fluidCircuit->SetNextAndCurrentFromBaselines();
  fluidCircuit->StateChange();

  bool serialized = false;
  while (currentTime_s < 1.0)
  {
    //Process
    fluidCalculator.Process(*fluidCircuit, timeStep_s);
    //PostProcess
    fluidCalculator.PostProcess(*fluidCircuit);
    currentTime_s += timeStep_s;
    trk1.Track(currentTime_s, *fluidCircuit);
    if (!serialized && currentTime_s > 0.8)
    {
      serialized = true;
      std::string pbaDir = sTestDirectory + "/PreChargeComplianceZeroVolume.pba"; 
      TestCircuitSerialization(pbaDir);
      fluidCircuit = m_Circuits->GetFluidCircuit("Fluid");
    }
  }
  std::string sOutputFile1 = sTestDirectory + "/PreChargeComplianceZeroVolume.txt";
  trk1.WriteTrackToFile(sOutputFile1.c_str());
  m_Circuits->Clear();
}

void CommonDataModelTest::PreChargeComplianceNonZeroVolume(const std::string& sTestDirectory)
{
  std::cout << "PreChargeComplianceNonZeroVolume\n";
  m_Logger->ResetLogFile(sTestDirectory + "/PreChargeComplianceNonZeroVolume.log");
  TimingProfile p;
  double timeStep_s = 1.0 / 165.0;
  double currentTime_s = 0.0;
  DataTrack trk1;

  SEFluidCircuitCalculator fluidCalculator(m_Logger);
  SEFluidCircuit* fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");

  SEFluidCircuitNode& node1 = fluidCircuit->CreateNode("node1");
  node1.GetPressure().SetValue(10.0, PressureUnit::mmHg);
  SEFluidCircuitNode& ground = fluidCircuit->CreateNode("node2");
  fluidCircuit->AddReferenceNode(ground);
  ground.GetNextPressure().SetValue(0.0, PressureUnit::mmHg);
  SEFluidCircuitPath& groundTonode1 = fluidCircuit->CreatePath(ground, node1, "groundTonode1");
  groundTonode1.GetPressureSourceBaseline().SetValue(10.0, PressureUnit::mmHg);
  SEFluidCircuitPath& node1Toground = fluidCircuit->CreatePath(node1, ground, "node1Toground");
  node1Toground.GetComplianceBaseline().SetValue(1.0, FlowComplianceUnit::mL_Per_mmHg);
  node1Toground.GetSourceNode().GetVolumeBaseline().SetValue(10.0, VolumeUnit::mL);
  fluidCircuit->SetNextAndCurrentFromBaselines();
  fluidCircuit->StateChange();

  bool serialized = false;
  while (currentTime_s < 1.0)
  {
    //Process
    fluidCalculator.Process(*fluidCircuit, timeStep_s);
    //PostProcess
    fluidCalculator.PostProcess(*fluidCircuit);
    currentTime_s += timeStep_s;
    trk1.Track(currentTime_s, *fluidCircuit);
    if (!serialized && currentTime_s > 0.8)
    {
      serialized = true;
      std::string pbaDir = sTestDirectory + "/PreChargeComplianceNonZeroVolume.pba"; 
      TestCircuitSerialization(pbaDir);
      fluidCircuit = m_Circuits->GetFluidCircuit("Fluid");
    }
  }
  std::string sOutputFile2 = sTestDirectory + "/PreChargeComplianceNonZeroVolume.txt";
  trk1.WriteTrackToFile(sOutputFile2.c_str());
  m_Circuits->Clear();
}

void CommonDataModelTest::CircuitLockingTest(const std::string& sOutputDirectory)
{
  SETestReport testReport(m_Logger);  
  SETestSuite& testSuite = testReport.CreateTestSuite();
  testSuite.SetName("Locking");
  SETestCase* testCase;
 
  TimingProfile pTimer;
  pTimer.Start("Suite");
  
  std::cout << "CircuitLockingTestTest\n";

  m_Logger->ResetLogFile(sOutputDirectory + "/CircuitLockingTest.log");
 
  SEFluidCircuit* fluidCircuit = &m_Circuits->CreateFluidCircuit("Fluid");
  //-----------------------------------------------------------
  //Nodes
  SEFluidCircuitNode& Node1 = fluidCircuit->CreateNode("Node1");
  SEFluidCircuitNode& Node2 = fluidCircuit->CreateNode("Node2");
  SEFluidCircuitNode& Node3 = fluidCircuit->CreateNode("Node3");
  SEFluidCircuitNode& Node4 = fluidCircuit->CreateNode("Node4");
  SEFluidCircuitNode& Node5 = fluidCircuit->CreateNode("Node5");

  Node3.GetNextPressure().SetValue(1, PressureUnit::Pa);
  Node3.GetPressure().SetValue(1, PressureUnit::Pa);

  //Reference node
  fluidCircuit->AddReferenceNode(Node5);
  Node5.GetNextPressure().SetValue(0, PressureUnit::Pa);
  //-----------------------------------------------------------
  //Paths
  SEFluidCircuitPath& inductor = fluidCircuit->CreatePath(Node1, Node2, "Inductor");
  inductor.GetInductanceBaseline().SetValue(25, FlowInertanceUnit::Pa_s2_Per_m3);
  SEFluidCircuitPath& resistor = fluidCircuit->CreatePath(Node2, Node3, "Resistor");
  resistor.GetResistanceBaseline().SetValue(25, FlowResistanceUnit::Pa_s_Per_m3);
  SEFluidCircuitPath& capacitor = fluidCircuit->CreatePath(Node3, Node4, "Capacitor");
  capacitor.GetComplianceBaseline().SetValue(0.01, FlowComplianceUnit::m3_Per_Pa);
  capacitor.GetSourceNode().GetVolumeBaseline().SetValue(10, VolumeUnit::m3);
  capacitor.GetFlow().SetValue(1, VolumePerTimeUnit::m3_Per_s);
  capacitor.GetNextFlow().SetValue(1, VolumePerTimeUnit::m3_Per_s);
  SEFluidCircuitPath& flowSource = fluidCircuit->CreatePath(Node4, Node5, "Flow Source");
  flowSource.GetFlowSourceBaseline().SetValue(0.1, VolumePerTimeUnit::m3_Per_s);
  SEFluidCircuitPath& potentialSource = fluidCircuit->CreatePath(Node5, Node1, "Potential Source");
  potentialSource.GetPotentialSourceBaseline().SetValue(10, PressureUnit::Pa);
  SEFluidCircuitPath& dummyPath = fluidCircuit->CreatePath(Node1, Node3, "Short");
  
  fluidCircuit->SetNextAndCurrentFromBaselines();
  fluidCircuit->StateChange();


  // No need to calc the circuit since we already set some values on it
  // Lock the circuit
  m_Circuits->SetReadOnly(true);
  std::string type = " Locked Before Serialization";

  bool caught = false;
  //Try to Change things we should not be able to change
  //First time through (i = 0) locked circuit
  //Second time through (i = 1) serialized locked circuit
  //Third time through (i = 2) unlocked circuit
  for (int i = 0; i <= 2; i++)
  {
    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("VolumeBaseline"+type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Capacitor")->GetSourceNode().GetVolumeBaseline().IncrementValue(2, VolumeUnit::m3); }
    catch (...) {
      caught = true; }
    if (i!=2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked VolumeBaseline");
    if (i==2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked VolumeBaseline");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("Volume" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Capacitor")->GetSourceNode().GetVolume().IncrementValue(2, VolumeUnit::m3); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked Volume");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked Volume");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("NextVolume" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Capacitor")->GetSourceNode().GetNextVolume().IncrementValue(2, VolumeUnit::m3);
    }
    catch (...) {
      caught = true;
    }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked NextVolume");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked Volume");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    // I SHOULD BE ALBE TO CHANGE THIS, AS THERE IS NO COMPLIANCE ON THE PATH
    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("No Capacitance NextVolume" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Short")->GetSourceNode().GetNextVolume().IncrementValue(2, VolumeUnit::m3);
    }
    catch (...) {
      caught = true;
    }
    if (caught)
      testCase->AddFailure("I should be able to modify a node Volume that has no compliance on any of its connected paths");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("Pressure" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Capacitor")->GetSourceNode().GetPressure().IncrementValue(2, PressureUnit::Pa); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked Pressure");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked Pressure");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("NextPressure" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Capacitor")->GetSourceNode().GetNextPressure().IncrementValue(2, PressureUnit::Pa); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked NextPressure");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked NextPressure");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("Flow" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Capacitor")->GetFlow().IncrementValue(2, VolumePerTimeUnit::m3_Per_s); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked Flow");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked Flow");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("NextFlow" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Capacitor")->GetNextFlow().IncrementValue(2, VolumePerTimeUnit::m3_Per_s); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked NextFlow");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked NextFlow");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("FlowSource" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Flow Source")->GetFlowSource().IncrementValue(2, VolumePerTimeUnit::m3_Per_s); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked FlowSource");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked FlowSource");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("Resistance" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Resistor")->GetResistance().IncrementValue(2, FlowResistanceUnit::Pa_s_Per_m3); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked Resistance");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked Resistance");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("Capacitance" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Capacitor")->GetCapacitance().IncrementValue(2, FlowComplianceUnit::m3_Per_Pa); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked Capacitance");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked Capacitance");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("Inductance" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Inductor")->GetInductance().IncrementValue(2, FlowInertanceUnit::Pa_s2_Per_m3); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked Inductance");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked Inductance");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    caught = false;
    testCase = &testSuite.CreateTestCase();
    testCase->SetName("PotentialSource" + type);
    pTimer.Start("Test");
    try {
      fluidCircuit->GetPath("Potential Source")->GetPotentialSource().IncrementValue(2, PressureUnit::Pa); }
    catch (...) {
      caught = true; }
    if (i != 2 && !caught)
      testCase->AddFailure("I should not be able to modify a locked PotentialSource");
    if (i == 2 && caught)
      testCase->AddFailure("I should be able to modify an unlocked PotentialSource");
    testCase->GetDuration().SetValue(pTimer.GetElapsedTime_s("Test"), TimeUnit::s);

    if (i == 0)
    {
      // Serialize and try again
      std::string pbaDir = sOutputDirectory + "/CircuitLockingCircuit.pba";
      TestCircuitSerialization(pbaDir);
      fluidCircuit = m_Circuits->GetFluidCircuit("Fluid");
      type = " Locked After Serialization";
    }
    else if (i == 1)
    {
      m_Circuits->SetReadOnly(false);
      type = " Unlocked";
    }
  }

  std::string sOutputFile = sOutputDirectory + "/CircuitLockingTestReport.pba";
  testReport.WriteFile(sOutputFile);
  m_Circuits->Clear();
}