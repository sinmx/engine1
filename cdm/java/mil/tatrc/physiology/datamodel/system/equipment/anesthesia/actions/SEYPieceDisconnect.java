/* Distributed under the Apache License, Version 2.0.
   See accompanying NOTICE file for details.*/

package mil.tatrc.physiology.datamodel.system.equipment.anesthesia.actions;

import com.kitware.physiology.cdm.AnesthesiaMachineActions.YPieceDisconnectData;

import mil.tatrc.physiology.datamodel.properties.SEScalar0To1;

public class SEYPieceDisconnect extends SEAnesthesiaMachineAction
{
  protected SEScalar0To1 severity;
  
  public SEYPieceDisconnect()
  {
    severity = null;
  }
  
  public void reset()
  {
    super.reset();
    if (severity != null)
      severity.invalidate();
  }
  
  public boolean isValid()
  {
    return hasSeverity();
  }
  
  public static void load(YPieceDisconnectData src, SEYPieceDisconnect dst)
  {
    SEAnesthesiaMachineAction.load(src.getAnesthesiaMachineAction(),dst);
    if(src.hasSeverity())
      SEScalar0To1.load(src.getSeverity(),dst.getSeverity());
  }
  public static YPieceDisconnectData unload(SEYPieceDisconnect src)
  {
    YPieceDisconnectData.Builder dst = YPieceDisconnectData.newBuilder();
    unload(src,dst);
    return dst.build();
  }
  protected static void unload(SEYPieceDisconnect src, YPieceDisconnectData.Builder dst)
  {
    SEAnesthesiaMachineAction.unload(src, dst.getAnesthesiaMachineActionBuilder());
    if (src.hasSeverity())
      dst.setSeverity(SEScalar0To1.unload(src.severity));
  }
  
  /*
   * Severity
   */
  public boolean hasSeverity()
  {
    return severity == null ? false : severity.isValid();
  }
  public SEScalar0To1 getSeverity()
  {
    if (severity == null)
      severity = new SEScalar0To1();
    return severity;
  }
  
  public String toString()
  {
    if (severity != null)
      return "YPiece Disconnect"
          + "\n\tSeverity: " + getSeverity();
    else
      return "Action not specified properly";
  }
}
