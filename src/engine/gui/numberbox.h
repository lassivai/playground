#pragma once

#include "textbox.h"
//#include <ctgmath>
/* TODO
 * - use template to handle differnt variable types
 * - also for vectors and ranges
 * - value type with template
 */


struct NumberBox : public TextBox
{
  static const int FLOATING_POINT = 0, INTEGER = 1;
  enum IncrementMode { Default, Linear, Power, LinearAboveOne };

  int numberType;
  bool restrictRange = false;
  double minValue, maxValue;

  //bool linearIncrement = false;
  IncrementMode incrementMode = IncrementMode::Default;
  double linearIncrementAmount = 1.0;
  double power = 2.0;;

  bool keyboardInputEnabled = true;
  bool mouseInputEnabled = true;

  int defaultIncrementModePowerShift = 0;

  std::string formatStringInt = "";
  std::string formatStringDouble = "";
  
  int numberOfDecimals = 6;
  
  bool autoHideDecimals = false;

  Vec2d validValueRange = Vec2d(1, -1);
  
  void checkIsValidValue() {
    double value = 0;
    toDouble(inputText, value);
    setValidValue(validValueRange.y < validValueRange.x || (value >= validValueRange.x && value <= validValueRange.y));
  }
  
  void setValidValueRange(double min, double max) {
    validValueRange.x = min;
    validValueRange.y = max;
    checkIsValidValue();
  }
  
  void setNumberOfDecimals(int numberOfDecimals) {
    this->numberOfDecimals = numberOfDecimals;
    formatStringDouble = "%." + std::to_string(numberOfDecimals) + "f";
  }

  void setLimits(double minValue, double maxValue) {
    if(minValue == this->minValue && maxValue == this->maxValue) return;
    
    this->minValue = minValue;
    this->maxValue = maxValue;
    if(numberType == FLOATING_POINT) {
      double value;
      getValue((void*)&value);
      if(value < minValue || value > maxValue) {
        if(value < minValue) value = minValue;
        if(value > maxValue) value = maxValue;
        onValueChange(this);
      }
    }
    if(numberType == INTEGER) {
      int value;
      getValue((void*)&value);
      if(value < (int)minValue || value > (int)maxValue) {
        if(value < (int)minValue) value = (int)minValue;
        if(value > (int)maxValue) value = (int)maxValue;
        onValueChange(this);
      }
    }
  }

  virtual ~NumberBox() {}

  NumberBox(std::string labelText, double inputNumber, int type, double minValue = 0, double maxValue = -1, double x = 0, double y = 0, int inputTextWidth = 15, int numberOfDecimals = 6, bool autoHideDecimals = false) : TextBox(labelText, "", x, y, inputTextWidth) {
    if(minValue <= maxValue) {
      this->minValue = minValue;
      this->maxValue = maxValue;
      restrictRange = true;
    }
    this->numberType = type;
    
    if(numberOfDecimals != 6) {
      setNumberOfDecimals(numberOfDecimals);
    }
    this->autoHideDecimals = autoHideDecimals;
    
    setInputText(inputNumber);
  }

  NumberBox(std::string labelText, double inputNumber, int type, double minValue, double maxValue, LayoutPlacer &layoutPlacer, int inputTextWidth = 15, int numberOfDecimals = 6, bool autoHideDecimals = false) : TextBox(labelText, "", layoutPlacer, inputTextWidth) {
    if(minValue <= maxValue) {
      this->minValue = minValue;
      this->maxValue = maxValue;
      restrictRange = true;
    }
    this->numberType = type;
    
    if(numberOfDecimals != 6) {
      setNumberOfDecimals(numberOfDecimals);
    }
    this->autoHideDecimals = autoHideDecimals;
    
    setInputText(inputNumber);
  }
  
  
  
  /*NumberBox(std::string labelText, double inputNumber, int type, double minValue = 0, double maxValue = -1, double x = 0, double y = 0, int inputTextWidth = 15, double paddingX = 0, double paddingY = 0, double inputPaddingX = 0, double inputPaddingY = 0) : TextBox(labelText, "", x, y, inputTextWidth, paddingX, paddingY, inputPaddingX, inputPaddingY) {
    if(minValue <= maxValue) {
      this->minValue = minValue;
      this->maxValue = maxValue;
      restrictRange = true;
    }
    this->numberType = type;
    setInputText(inputNumber);
  }*/

  void setInputEnabled(bool inputEnabled) {
    keyboardInputEnabled = inputEnabled;
    mouseInputEnabled = inputEnabled;
  }
  
  void setInputText(double value) {
    if(numberType == INTEGER) {
      if(formatStringInt.size() > 0) {
        inputText = format(formatStringInt, (int)value);
      }
      else {
        inputText = std::to_string((int)value);
      }
    }
    else {
      if(autoHideDecimals) {
        int mag = max(0, (int)log10(value));
        
        int numberOfDecimalsTmp = numberOfDecimals;
        setNumberOfDecimals(max(0, numberOfDecimals - mag));
        inputText = format(formatStringDouble, value);
        numberOfDecimals = numberOfDecimalsTmp;
      }
      
      if(formatStringDouble.size() > 0) {
        inputText = format(formatStringDouble, value);
      }
      else {
        inputText = std::to_string(value);
      }
    }
  }
  /*void onKeyDown(Events &events) {
    if(isInputGrabbed) {
      if(events.sdlKeyCode == SDLK_MINUS) {

      }
    }
  }*/

  /*void onKeyDown(Events &events) {
    GuiElement::onKeyDown(events);
    if(isInputGrabbed) {
      if(events.sdlKeyCode == SDLK_RETURN) {
        isInputGrabbed = false;
        onAction(this);
      }
    }
  }*/

  void onTextInput(Events &events) {
    if(isInputGrabbed && keyboardInputEnabled) {
      std::string previousInputText = inputText;
      if(numberType == INTEGER) {
        int i;
        if(toInt(events.textInput.inputText, i)) {
          if(restrictRange && i < minValue) {
            setInputText((int)minValue);
          }
          else if(restrictRange && i > maxValue) {
            setInputText((int)maxValue);
          }
          else {
            setInputText(i);
          }
        }
        else {
          if(restrictRange) {
            setInputText((int)minValue);
          }
          else {
            setInputText(0);
          }
        }
        events.textInput.inputText = inputText;
      }
      else {
        double d;
        if(toDouble(events.textInput.inputText, d)) {
          if(events.textInput.inputText.size() == 0 || (restrictRange && d < minValue)) {
            setInputText(minValue);
          }
          else if(restrictRange && d > maxValue) {
            setInputText(maxValue);
          }
          else {
            setInputText(d);
          }
        }
        else {
          if(restrictRange) {
            setInputText(minValue);
          }
          else {
            setInputText(0);
          }
        }
        events.textInput.inputText = inputText;
      }
      textToCursorTmp = events.textInput.getTextToCursor();
      if(inputText.compare(previousInputText) != 0) {
        checkIsValidValue();
        onValueChange(this);
      }
    }
  }

  void getValue(void *value) {
    if(numberType == INTEGER) {
      int i;
      toInt(inputText, i);
      *(int*)value = i;
    }
    else {
      double d;
      toDouble(inputText, d);
      *(double*)value = d;
    }
  }

  virtual void getTypedValue(double &value) {
    toDouble(inputText, value);
  }
  virtual void getTypedValue(int &value) {
    toInt(inputText, value);
  }

  void setValue(std::string valueStr) {
    prerenderingNeeded = true;

    if(numberType == INTEGER) {
      int i;
      if(toInt(valueStr, i)) {
        if(valueStr.size() == 0 || (restrictRange && i < minValue)) {
          //inputText = std::to_string((int)minValue);
          setInputText((int)minValue);
        }
        else if(restrictRange && i > maxValue) {
          //inputText = std::to_string((int)maxValue);
          setInputText((int)maxValue);
        }
        else {
          //inputText = std::to_string(i);
          setInputText(i);
        }
      }
      else {
        if(restrictRange) {
          //inputText = std::to_string((int)minValue);
          setInputText((int)minValue);
        }
        else {
          //inputText = std::to_string(0);
          setInputText(0);
        }
      }
      //events.textInput.inputText = inputText;
    }
    else {
      double d;
      if(toDouble(valueStr, d)) {
        if(valueStr.size() == 0 || (restrictRange && d < minValue)) {
          //inputText = std::to_string(minValue);
          setInputText(minValue);
        }
        else if(restrictRange && d > maxValue) {
          //inputText = std::to_string(maxValue);
          setInputText(maxValue);
        }
        else {
          //inputText = std::to_string(d);
          setInputText(d);
        }
      }
      else {
        if(restrictRange) {
          //inputText = std::to_string(minValue);
          setInputText(minValue);
        }
        else {
          //inputText = std::to_string(0);
          setInputText(0);
        }
      }
      //events.textInput.inputText = inputText;
    }
    //textToCursorTmp = events.textInput.getTextToCursor();
    checkIsValidValue();
  }

  /*void setValue(void *value) {
    if(numberType == INTEGER) {
      int *i = (int*)value;
      if(restrictRange && *i < minValue) {
        inputText = std::to_string((int)minValue);
      }
      else if(restrictRange && *i > maxValue) {
        inputText = std::to_string((int)maxValue);
      }
      else {
        inputText = std::to_string(*i);
      }
    }
    else {
      double *d = (double*)value;
      if(restrictRange && *d < minValue) {
        inputText = std::to_string(minValue);
      }
      else if(restrictRange && *d > maxValue) {
        inputText = std::to_string(maxValue);
      }
      else {
        inputText = std::to_string(*d);
      }
    }
  }*/

  void setValue(double value) {
    std::string previousInputText = inputText;

    if(numberType == INTEGER) {
      int i = (int)value;
      if(restrictRange && i < minValue) {
        //inputText = std::to_string((int)minValue);
        setInputText((int)minValue);
      }
      else if(restrictRange && i > maxValue) {
        //inputText = std::to_string((int)maxValue);
        setInputText((int)maxValue);
      }
      else {
        //inputText = std::to_string(i);
        setInputText(i);
      }
    }
    else {
      if(restrictRange && value < minValue) {
        //inputText = std::to_string(minValue);
        setInputText(minValue);
      }
      else if(restrictRange && value > maxValue) {
        //inputText = std::to_string(maxValue);
        setInputText(maxValue);
      }
      else {
        //inputText = std::to_string(value);
        setInputText(value);
      }
    }

    if(inputText.compare(previousInputText) != 0) {
      checkIsValidValue();
      prerenderingNeeded = true;
    }
    
  }


  void onMouseWheel(Events &events) {
    GuiElement::onMouseWheel(events);

    if(!isInputGrabbed || !mouseInputEnabled) return;

    std::string previousInputText = inputText;

    if(numberType == INTEGER) {
      int value;
      toInt(inputText, value);
      if(incrementMode == IncrementMode::Power) {
        value = (int)(value * pow(power, events.mouseWheel));
      }
      else {
        int increment = (int)pow(10, inputText.size()-1+defaultIncrementModePowerShift);
        if(incrementMode == IncrementMode::Linear) {
          increment = (int)round(linearIncrementAmount);
          if(events.lControlDown) increment *= 10;
        }

        value += events.mouseWheel * increment;
        if(value == 0) {
          value = increment * 9 / 10;
        }
      }
      value = clamp(value, (int)minValue, (int)maxValue);
      //inputText = std::to_string(value);
      setInputText(value);
    }
    else {
      double value;
      toDouble(inputText, value);
      //printf("debugging at NumberBox::onMouseWheel() 1, %f, '%s'\n", value, inputText.c_str());
      /*if(isnan(value)) {
        value = 0;
        inputText = "0.000000";
      }*/

      int m = 1;
      IncrementMode incrementMode = this->incrementMode;
      if(events.rAltDown) {
        if(incrementMode == IncrementMode::Power) incrementMode = IncrementMode::Linear;
        else if(incrementMode == IncrementMode::Linear) incrementMode = IncrementMode::Power;
      }

      if(incrementMode == IncrementMode::Power) {
        value = value * pow(power, events.mouseWheel);
      }
      else if(events.rShiftDown && events.rControlDown) {
        value = value * pow(2.0, 1.0/12.0 * events.mouseWheel);
      }
      else {
        double increment = 0;
        //printf("debugging at NumberBox::onMouseWheel() 1.1, %f, '%s', %f\n", value, inputText.c_str(), increment);
        bool linearIncrement = incrementMode == IncrementMode::Linear || (incrementMode == IncrementMode::LinearAboveOne && value > 1.0);
        if(events.lAltDown) linearIncrement = !linearIncrement;

        if(linearIncrement) {
          increment = linearIncrementAmount;
        }
        else {
          int k = -1;
          int z = -1;
          for(int i=0; i<inputText.size(); i++) {
            if(inputText[i] == ',' || inputText[i] == '.') {
              //z = i + m;
              z = i;
            }
            else if(inputText[i] == '-' && z == -1) {
              m = -1;
            }
            else if(inputText[i] != '0' && k == -1)  {
              k = i;
            }
          }
          if(k >= 0 && z >= 0) {
            if(k < z) {
              increment = pow(10.0, z-k-1+defaultIncrementModePowerShift);
              //printf("debugging at NumberBox::onMouseWheel() 1.2, %f, '%s', %f\n", value, inputText.c_str(), increment);
            }
            else {
              increment = pow(10.0, z-k+defaultIncrementModePowerShift);
              //printf("debugging at NumberBox::onMouseWheel() 1.3, %f, '%s', %f\n", value, inputText.c_str(), increment);
            }
          }
          else if(k >= 0) {
            increment = pow(10.0, inputText.size()-1-k+defaultIncrementModePowerShift);
            //printf("debugging at NumberBox::onMouseWheel() 1.4, %f, '%s', %f\n", value, inputText.c_str(), increment);
          }
        }
        //printf("debugging at NumberBox::onMouseWheel() 2, %f, '%s', %f\n", value, inputText.c_str(), increment);
        if(events.lControlDown) increment *= 0.1;
        if(events.lShiftDown) increment *= 0.01;

        value += events.mouseWheel * increment;
        //printf("debugging at NumberBox::onMouseWheel() 3, %f, '%s', %f\n", value, inputText.c_str(), increment);
        
        if(!linearIncrement && value == 0.0) {
          //printf("debugging at NumberBox::onMouseWheel() 4, %f, '%s', %f\n", value, inputText.c_str(), increment);
          if(increment != 0) {
            //printf("debugging at NumberBox::onMouseWheel() 5, %f, '%s', %f\n", value, inputText.c_str(), increment);
            value = m * increment * 0.9;
            //printf("debugging at NumberBox::onMouseWheel() 6, %f, '%s', %f\n", value, inputText.c_str(), increment);
          }
          else {
            //printf("debugging at NumberBox::onMouseWheel() 7, %f, '%s', %f\n", value, inputText.c_str(), increment);
            double k = pow(10.0, -numberOfDecimals);
            value = events.mouseWheel < 0 ? -k : k;
            //printf("debugging at NumberBox::onMouseWheel() 8, %f, '%s', %f\n", value, inputText.c_str(), increment);
          }
        }

      }
      //printf("debugging at NumberBox::onMouseWheel() 9, %f, '%s'\n", value, inputText.c_str());
      value = clamp(value, minValue, maxValue);
      //printf("debugging at NumberBox::onMouseWheel() 10, %f, '%s'\n", value, inputText.c_str());
      //inputText = std::to_string(value);
      setInputText(value);
      //printf("debugging at NumberBox::onMouseWheel() 11, %f, '%s'\n", value, inputText.c_str());
    }

    if(inputText.compare(previousInputText) != 0) {
      events.textInput.setInputText(inputText);
      checkIsValidValue();
      onValueChange(this);
    }
  }

};
