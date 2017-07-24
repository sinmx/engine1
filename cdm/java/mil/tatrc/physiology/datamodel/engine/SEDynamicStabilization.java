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
package mil.tatrc.physiology.datamodel.engine;

import java.util.*;

import com.google.protobuf.TextFormat;
import com.google.protobuf.TextFormat.ParseException;
import com.kitware.physiology.cdm.Engine.DynamicStabilizationData;
import com.kitware.physiology.cdm.PatientNutrition.NutritionData;
import com.kitware.physiology.cdm.Properties.eSwitch;

import mil.tatrc.physiology.datamodel.patient.nutrition.SENutrition;
import mil.tatrc.physiology.datamodel.properties.SEScalarTime;
import mil.tatrc.physiology.utilities.FileUtils;

public class SEDynamicStabilization
{
  protected eSwitch                                             trackingStabilization;
  protected SEDynamicStabilizationEngineConvergence             restingConvergence;
  protected SEDynamicStabilizationEngineConvergence             feedbackConvergence;
  protected Map<String,SEDynamicStabilizationEngineConvergence> conditionConvergence;
  
  public SEDynamicStabilization()
  {
    this.conditionConvergence=new HashMap<String,SEDynamicStabilizationEngineConvergence>();
  }
  
  public void reset()
  {
    this.trackingStabilization=eSwitch.Off;
    this.restingConvergence=null;
    this.feedbackConvergence=null;
    this.conditionConvergence.clear();
  }
  
  public void readFile(String fileName) throws ParseException
  {
    DynamicStabilizationData.Builder builder = DynamicStabilizationData.newBuilder();
    TextFormat.getParser().merge(FileUtils.readFile(fileName), builder);
    SEDynamicStabilization.load(builder.build(), this);
  }
  public void writeFile(String fileName)
  {
    FileUtils.writeFile(fileName, SEDynamicStabilization.unload(this).toString());
  }
  
  public static void load(DynamicStabilizationData src, SEDynamicStabilization dst) 
  {
    if(src.getTrackingStabilization()!=eSwitch.UNRECOGNIZED)
      dst.trackingStabilization=src.getTrackingStabilization();
    if(src.hasRestingConvergence())
      SEDynamicStabilizationEngineConvergence.load(src.getRestingConvergence(),dst.getRestingConvergence());
    if(src.hasFeedbackConvergence())
      SEDynamicStabilizationEngineConvergence.load(src.getFeedbackConvergence(),dst.getFeedbackConvergence());
    for(String name : src.getConditionConvergenceMap().keySet())
    {
      SEDynamicStabilizationEngineConvergence.load(src.getConditionConvergenceMap().get(name), dst.createConditionConvergence(name));
    }
  }
  public static DynamicStabilizationData unload(SEDynamicStabilization src)
  {
    DynamicStabilizationData.Builder dst = DynamicStabilizationData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SEDynamicStabilization src, DynamicStabilizationData.Builder dst)
  {
    if(src.hasTrackingStabilization())
      dst.setTrackingStabilization(src.trackingStabilization);
    if(src.hasRestingConvergence())
      dst.setRestingConvergence(SEDynamicStabilizationEngineConvergence.unload(src.restingConvergence));
    if(src.hasFeedbackConvergence())
      dst.setFeedbackConvergence(SEDynamicStabilizationEngineConvergence.unload(src.feedbackConvergence));
    for(String name : src.conditionConvergence.keySet())
    {
    	dst.getConditionConvergenceMap().put(name, SEDynamicStabilizationEngineConvergence.unload(src.conditionConvergence.get(name)));
    }
  }
  
  public boolean hasTrackingStabilization()
  {
    return this.trackingStabilization!=null;
  }
  public eSwitch isTrackingStabilization()
  {
    return this.trackingStabilization;
  }
  public void TrackStabilization(eSwitch b)
  {
    this.trackingStabilization=b;
  }
  
  public boolean hasRestingConvergence()
  {
    return restingConvergence != null;
  }
  public SEDynamicStabilizationEngineConvergence getRestingConvergence()
  {
    if (restingConvergence == null)
      restingConvergence = new SEDynamicStabilizationEngineConvergence();
    return restingConvergence;
  }
  
  public boolean hasFeedbackConvergence()
  {
    return feedbackConvergence != null;
  }
  public SEDynamicStabilizationEngineConvergence getFeedbackConvergence()
  {
    if (feedbackConvergence == null)
    	feedbackConvergence = new SEDynamicStabilizationEngineConvergence();
    return feedbackConvergence;
  }
  
  public SEDynamicStabilizationEngineConvergence createConditionConvergence(String type)
  {
    SEDynamicStabilizationEngineConvergence c = new SEDynamicStabilizationEngineConvergence();
    this.conditionConvergence.put(type, c);
    return c;
  }
  public boolean hasConditionConvergence(String type)
  {
    return this.conditionConvergence.containsKey(type);
  }
  public SEDynamicStabilizationEngineConvergence getConditionConvergence(String type)
  {
    return this.conditionConvergence.get(type);
  }
}
