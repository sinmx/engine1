/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package mil.tatrc.physiology.datamodel.properties;

import com.kitware.physiology.cdm.Properties.ScalarFlowResistanceData;

import mil.tatrc.physiology.datamodel.properties.CommonUnits.FlowResistanceUnit;

/**
 * @author abray
 * FlowResistance Scalar Class, will enforce that units are proper to FlowResistances
 */

public class SEScalarFlowResistance extends SEScalar
{
  public SEScalarFlowResistance()
  {
    super();
  }
  
  /**
   * @param value
   * @param unit - specific any valid standard unit abbreviation
   * http://www.bipm.org/en/si/ for this quantity type
   */
  public SEScalarFlowResistance(double value, String unit)
  {
    this();
    this.setValue(value, unit);
  }
  
  /**
   * @param value
   * @param unit - enumeration of commonly used units for this type
   * Units are not limited to this set.
   * You can use the other method convention:(double,string)
   * and specify any valid standard unit abbreviation sting for this type
   * Unit Abbreviation Standards: http://www.bipm.org/en/si/
   */
  public SEScalarFlowResistance(double value, FlowResistanceUnit unit)
  {
    this();
    this.setValue(value,unit);
  }
  
  public static void load(ScalarFlowResistanceData src, SEScalarFlowResistance dst)
  {
    SEScalar.load(src.getScalarFlowResistance(),dst);
  }
  public static ScalarFlowResistanceData unload(SEScalarFlowResistance src)
  {
    if(!src.isValid())
      return null;
    ScalarFlowResistanceData.Builder dst = ScalarFlowResistanceData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SEScalarFlowResistance src, ScalarFlowResistanceData.Builder dst)
  {
    SEScalar.unload(src,dst.getScalarFlowResistanceBuilder());
  }
  
  /**
   * @param value
   * @param unit - enumeration of commonly used units for this type
   * Units are not limited to this set.
   * You can use the other method convention:(double,string)
   * and specify any valid standard unit abbreviation sting for this type
   * Unit Abbreviation Standards: http://www.bipm.org/en/si/
   */
  public void setValue(double value, FlowResistanceUnit unit)
  {
    this.setValue(value,unit.toString());
  }
  
  /**
   * @param value
   * @param unit - enumeration of commonly used units for this type
   * Units are not limited to this set.
   * You can use the other method convention:(double,string)
   * and specify any valid standard unit abbreviation sting for this type
   * Unit Abbreviation Standards: http://www.bipm.org/en/si/
   */
  public double getValue(FlowResistanceUnit unit)
  {
    return this.getValue(unit.toString());
  }

  public boolean validUnit(String unit)
  {
    if(FlowResistanceUnit.validUnit(unit))
      return true;
    return false;
  }
}
