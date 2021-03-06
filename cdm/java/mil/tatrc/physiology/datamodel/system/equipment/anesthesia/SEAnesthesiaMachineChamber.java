/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/
package mil.tatrc.physiology.datamodel.system.equipment.anesthesia;

import com.kitware.physiology.cdm.AnesthesiaMachine.AnesthesiaMachineData;
import com.kitware.physiology.cdm.Enums.eSwitch;

import mil.tatrc.physiology.datamodel.properties.SEScalar0To1;
import mil.tatrc.physiology.datamodel.substance.SESubstance;
import mil.tatrc.physiology.datamodel.substance.SESubstanceFractionAmount;
import mil.tatrc.physiology.datamodel.substance.SESubstanceManager;

public class SEAnesthesiaMachineChamber 
{
  protected eSwitch state;
  protected SESubstance substance;
  protected SEScalar0To1 substanceFraction;

  public SEAnesthesiaMachineChamber()
  {
    this.state = eSwitch.Off;
    this.substance = null;
    this.substanceFraction = null;
  }

  public void reset()
  {
    state = eSwitch.Off;
    if (substanceFraction != null)
      substanceFraction.invalidate();
  }

  public void copy(SEAnesthesiaMachineChamber from)
  {
    reset();
    this.state=from.state;
    this.substance=from.substance;
    if(from.hasSubstanceFraction())
      this.getSubstanceFraction().set(from.substanceFraction);      
  }

  public static void load( AnesthesiaMachineData.ChamberData src, SEAnesthesiaMachineChamber dst, SESubstanceManager subMgr)
  {
    dst.reset();
    if (src.getState() != eSwitch.UNRECOGNIZED && src.getState()!=eSwitch.NullSwitch)
      dst.setState(src.getState());
    if (src.getSubstance()!=null)
      dst.setSubstance(subMgr.getSubstance(src.getSubstance()));
    if (src.hasSubstanceFraction())
      SEScalar0To1.load(src.getSubstanceFraction(),dst.getSubstanceFraction());
  }
  public static AnesthesiaMachineData.ChamberData unload(SEAnesthesiaMachineChamber src)
  {
    AnesthesiaMachineData.ChamberData.Builder dst =  AnesthesiaMachineData.ChamberData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SEAnesthesiaMachineChamber src, AnesthesiaMachineData.ChamberData.Builder dst)
  {
    if(src.hasSubstance())
      dst.setSubstance(src.substance.getName());
    dst.setState(src.state);
    if (src.hasSubstanceFraction())
      dst.setSubstanceFraction(SEScalar0To1.unload(src.substanceFraction));
  }

  public eSwitch getState()
  {
    return state;
  }
  public void setState(eSwitch s)
  {
  	this.state = (s==eSwitch.NullSwitch) ? eSwitch.Off : s;
  }

  public boolean hasSubstanceFraction()
  {
    return substanceFraction == null ? false : substanceFraction.isValid();
  }
  public SEScalar0To1 getSubstanceFraction()
  {
    if (substanceFraction == null)
      substanceFraction = new SEScalar0To1();
    return substanceFraction;
  }

  public boolean hasSubstance()
  {
    return this.substance!=null;
  }
  public SESubstance getSubstance()
  {
    return substance;
  }
  public void setSubstance(SESubstance substance)
  {
    this.substance = substance;
  }

  public String toString()
  {
      return "Anesthesia Machine Chamber"
      + "\n\tState: " + getState()
      + "\n\tSubstance Fraction: " + getSubstanceFraction()
      + "\n\tSubstance: " + (hasSubstance()?getSubstance().getName():"NotProvided");
  }
}
