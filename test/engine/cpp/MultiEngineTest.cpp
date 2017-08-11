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

#include "EngineTest.h"

#include "utils/TaskRunner/TaskRunner.h"
#include "utils/TaskRunner/Task.h"
#include "utils/FileUtils.h"

#include "scenario/SEScenarioExec.h"

class RunEngineTask : public Task
{
public:
  RunEngineTask(std::unique_ptr<PhysiologyEngine> pEngine)
    : m_pEngine(std::move(pEngine))
  {
  }

  virtual void Run() override
  {
    m_pEngine->InitializeEngine("StandardMale.pba");

    double dT_s = m_pEngine->GetTimeStep(TimeUnit::s);

    int count = static_cast<int>(10.0 / dT_s);
    for (int i = 0; i <= count; ++i)
    {
      m_pEngine->AdvanceModelTime();
    }
  }

private:
  std::unique_ptr<PhysiologyEngine> m_pEngine;
};

class RunScenarioTask : public Task
{
public:
  static std::mutex ms_initializationMutex;

  RunScenarioTask(const std::string& scenarioFile)
    : m_scenarioFile(scenarioFile)
  {
  }

  virtual void Run() override;

private:
  std::string m_scenarioFile;
};

std::mutex RunScenarioTask::ms_initializationMutex;

void RunScenarioTask::Run()
{
  // Set up the log file
  std::string logFile = m_scenarioFile;
  logFile = Replace(logFile, "verification", "bin");
  logFile = Replace(logFile, ".pba", ".log");
  // Set up the verification output file  
  std::string dataFile = m_scenarioFile;
  dataFile = Replace(dataFile, "verification", "bin");
  dataFile = Replace(dataFile, ".pba", "Results.txt");
  // Delete any results file that may be there
  remove(dataFile.c_str());

  ms_initializationMutex.lock();
  std::unique_ptr<PhysiologyEngine> Pulse = CreatePulseEngine(logFile.c_str());
  ms_initializationMutex.unlock();

  if (!Pulse)
  {
    std::cerr << "Unable to create PulseEngine" << std::endl;
    return;
  }
  SEScenarioExec exec(*Pulse);
  exec.Execute(m_scenarioFile.c_str(), dataFile.c_str(), NULL);
}

void PulseEngineTest::MultiEngineTest(const std::string& sTestDirectory)
{
  double singleThreadTime = 0.0;
  double multiThreadTime = 0.0;

  {
    TaskRunner runner(1);

    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/Basic1.pba")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/Basic2.pba")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/Basic3.pba")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/BasicCynthia.pba")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/BasicGus.pba")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/BasicHassan.pba")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/BasicJoel.pba")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/BasicNathan.pba")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/BasicSoldier.pba")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/BasicStandardHeavy.pba")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/BasicStandardLight.pba")));

    std::cout << " *** Running single-threaded ***" << "\n";

    TimingProfile timer;
    timer.Start("timer");

    runner.Run();

    timer.Stop("timer");
    singleThreadTime = timer.GetElapsedTime_s("timer");

    std::cout << " *** Single-threaded ran in " << timer.GetElapsedTime_s("timer") << " s\n";
  }

  {
    TaskRunner runner;

    std::cout << " *** Running multithreaded ***" << "\n";

    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/Basic1.xml")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/Basic2.xml")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/Basic3.xml")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/BasicCynthia.xml")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/BasicGus.xml")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/BasicHassan.xml")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/BasicJoel.xml")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/BasicNathan.xml")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/BasicSoldier.xml")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/BasicStandardHeavy.xml")));
    runner.AddTask(std::unique_ptr<RunScenarioTask>(new RunScenarioTask("../verification/Scenarios/Basic/BasicStandardLight.xml")));

    TimingProfile timer;
    timer.Start("timer");

    runner.Run();

    timer.Stop("timer");
    multiThreadTime = timer.GetElapsedTime_s("timer");

    std::cout << " *** Multithreaded ran in " << timer.GetElapsedTime_s("timer") << " s\n";
  }

  std::cout << "Done\n";

  std::cout << "\n"
    "=============================="
    "=============================="
    "=============================="
    "\n"
    "Single threaded time:      " << singleThreadTime << " s\n"
    "Multi threaded time:       " << multiThreadTime << " s\n"
    "Multi threaded comparison: "
    << std::fixed << std::setprecision(0) << ((multiThreadTime / singleThreadTime) * 100) << "%\n"
    "\n"
    "=============================="
    "=============================="
    "=============================="
    "\n";

  std::cin.get();
}