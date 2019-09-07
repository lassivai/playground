#pragma once

#include <ctgmath>
#include "../util.h"

enum EnvelopeType { AttackHoldExpDecay, AttackHoldExpDecay2, AttackDecaySustainRelease, FourPoints, FivePoints/*, AttackHoldPartialDecays*/, PowerAttackDecay };
static const std::vector<std::string> envelopeTypeShortNames = { "ahe", "ahe2", "adsr", "fourPoint", "fivePoint"/*, "ahpd"*/, "pad" };


struct EnvelopePreset {
  std::string name;
  EnvelopeType envelopeType;
  std::vector<double> args;

  EnvelopePreset() {}

  EnvelopePreset(const std::string &name, EnvelopeType envelopeType, const std::vector<double> &args) {
    this->name = name;
    this->envelopeType = envelopeType;
    this->args = std::vector<double>(args);
  }
};

static EnvelopePreset epAHE01("AHE 01", EnvelopeType::AttackHoldExpDecay, {0.025, -1, -6});
static EnvelopePreset epAHE02("AHE 02", EnvelopeType::AttackHoldExpDecay, {0.0, -0.8, -2});
static EnvelopePreset epAHE03("AHE 03", EnvelopeType::AttackHoldExpDecay, {0.0, -2., -5});
static EnvelopePreset epAHE04("AHE 04", EnvelopeType::AttackHoldExpDecay, {0.0, -0.008, -0.1});
static EnvelopePreset epAHE05("AHE 05", EnvelopeType::AttackHoldExpDecay, {0.0, -1., -2});
static EnvelopePreset epAHE06("AHE 06", EnvelopeType::AttackHoldExpDecay, {0.0, -4., -1});
static EnvelopePreset epAHE07("AHE 07", EnvelopeType::AttackHoldExpDecay, {0.01, -0.5, -3});
static EnvelopePreset epAHE08("AHE 08", EnvelopeType::AttackHoldExpDecay, {0.2, 0, -2});
static EnvelopePreset epAHE09("AHE 09", EnvelopeType::AttackHoldExpDecay, {0.9, -0.3, -1});
static EnvelopePreset epAHE10("AHE 10", EnvelopeType::AttackHoldExpDecay, {0.06, -1, -6});
static EnvelopePreset epAHE11("AHE 11", EnvelopeType::AttackHoldExpDecay, {0.0, -0.3, -10});
static EnvelopePreset epAHE12("AHE 12", EnvelopeType::AttackHoldExpDecay, {0.2, -0.02, -7});
static EnvelopePreset epADSR01("ADSR 01", EnvelopeType::AttackDecaySustainRelease, {0.06, 0.04, 0.5, 0.1});
static EnvelopePreset epFourpoint01("4-point 01", EnvelopeType::FourPoints, {1.0, 1.0, 0.1, 0.0, 0, 0.8, 0, 0.8});
static EnvelopePreset epFourpoint03("4-point 03", EnvelopeType::FourPoints, {1, 1, 1, 1, 0, 0, 0, 0});
static EnvelopePreset epFourpoint02("4-point 02", EnvelopeType::FourPoints, {3., 0.3, 0., 0.0, 0.8, 2.0, 0, 0.0});
static EnvelopePreset epFourpoint04("4-point 04", EnvelopeType::FourPoints, {0, 1, 1, 0, 0.02, 0, 0, 0.02});
static EnvelopePreset epFivepoint01("5-point 01", EnvelopeType::FivePoints, {1, 1, 0.4, 0, 0, 0, 0.1, 0, 6.0, 0.5});
static EnvelopePreset epFivepoint02("5-point 02", EnvelopeType::FivePoints, {1, 1, 0.5, 0, 0, 0, 0.3, 0, 4.0, 0.1});
static EnvelopePreset epFivepoint03("5-point 03", EnvelopeType::FivePoints, {0, 1, 1, 0, 0, 0.02, 0.07, 0, 4.0, 0.5});
//static EnvelopePreset epAHPD01("AHPD 01", EnvelopeType::AttackHoldPartialDecays, {0, -1, -8, 0.5, 0.5});
static EnvelopePreset epPAD01("PAD 01", EnvelopeType::PowerAttackDecay, {0, 0, 3, 5});
static EnvelopePreset epPAD02("PAD 02", EnvelopeType::PowerAttackDecay, {0, 0, 3, 0.4});
static EnvelopePreset epPAD03("PAD 03", EnvelopeType::PowerAttackDecay, {0, 0, 0.5, 3});
static EnvelopePreset epPAD04("PAD 04", EnvelopeType::PowerAttackDecay, {0, 0, 4, 3});
static EnvelopePreset epPAD05("PAD 05", EnvelopeType::PowerAttackDecay, {0, 0, 1, 5});
static EnvelopePreset epPAD06("PAD 06", EnvelopeType::PowerAttackDecay, {0, 0, 0.9, 7});
static EnvelopePreset epPAD07("PAD 07", EnvelopeType::PowerAttackDecay, {0, 0, 2, 7});
static EnvelopePreset epPAD08("PAD 08", EnvelopeType::PowerAttackDecay, {0, 0, 0.8, 40});
static EnvelopePreset epPAD09("PAD 09", EnvelopeType::PowerAttackDecay, {0, 0, 0.2, 6});
static EnvelopePreset epPAD10("PAD 10", EnvelopeType::PowerAttackDecay, {0, 0, 3, 10});
static EnvelopePreset epPAD11("PAD 11", EnvelopeType::PowerAttackDecay, {0, 0, 3, 20});

static std::vector<EnvelopePreset> envelopePresets = { epAHE01, epADSR01, epFourpoint01, epFourpoint02, epFourpoint03, epFivepoint01, epFivepoint02};


struct Envelope : public PanelInterface, public HierarchicalTextFileParser {
  
  double ahdAttackDuration = 0.0, ahdHoldDecayExponent = 0;
  double ahdReleaseDecayExponent = -6;
  double ahdOffset = -0.05;

  double ahd2AttackDuration = 0.0;
  double ahd2HoldDecayExponent = -1, ahd2HoldDecayPower = 0;
  double ahd2ReleaseDecayExponent = -6;
  double ahd2Offset = -0.05;

  double adsrAttackDuration = 0.0;
  double adsrDecayDuration = 0.1;
  double adsrSustainLevel = 1.0;
  double adsrReleaseDuration = 1.0;

  /*double ahpdAttackDuration = 0.0;
  double ahpdHoldDecayExponent = -1.0;
  double ahpdReleaseDecayExponent = -6.0;
  double ahpdPartialDecayCoefficient = 0.0;
  double ahpdPartialDecayCoefficient2 = 0.5;
  std::vector<double> ahpdPartialDecayFactors;*/

  std::vector<double> fourPointLevels = {1, 1, 1, 0};
  std::vector<double> fourPointDurations = {0, 0, 0, 0};

  std::vector<double> fivePointLevels = {1, 1, 0.5, 0.2, 0};
  std::vector<double> fivePointDurations = {0.1, 0.2, 0.2, 3, 0.5};
  /* levels: 0 start, 1 after attack, 2 after decay, 3 after hold, 4 after release */
  /* durations: 0 attack, 1 decay, 2 min hold, 3 max hold, 4 release */

  double padAttackDuration = 0.0;
  double padAttackPower = 0;
  double padReleaseDuration = 3;
  double padReleasePower = 5;

  EnvelopeType type = EnvelopeType::AttackHoldExpDecay;

  std::vector<double> envelopeGraph, envelopeSecondaryGraph;

  virtual ~Envelope() {}

  Envelope() {
    //ahpdPartialDecayFactors.resize(PartialSet::maxNumPartials);
    prepare();
  }

  inline void operator=(const Envelope &a) {
    this->type = a.type;
    
    this->ahdAttackDuration = a.ahdAttackDuration;
    this->ahdHoldDecayExponent = a.ahdHoldDecayExponent;
    this->ahdReleaseDecayExponent = a.ahdReleaseDecayExponent;
    this->ahdOffset = a.ahdOffset;

    this->ahd2AttackDuration = a.ahd2AttackDuration;
    this->ahd2HoldDecayPower = a.ahd2HoldDecayPower;
    this->ahd2HoldDecayExponent = a.ahd2HoldDecayExponent;
    this->ahd2ReleaseDecayExponent = a.ahd2ReleaseDecayExponent;
    this->ahd2Offset = a.ahd2Offset;

    
    //this->keyHoldEnabled = a.keyHoldEnabled;
    this->adsrAttackDuration = a.adsrAttackDuration;
    this->adsrDecayDuration = a.adsrDecayDuration;
    this->adsrSustainLevel = a.adsrSustainLevel;
    this->adsrReleaseDuration = a.adsrReleaseDuration;
    for(int i=0; i<4; i++) {
      this->fourPointLevels[i] = a.fourPointLevels[i];
      this->fourPointDurations[i] = a.fourPointDurations[i];
    }
    for(int i=0; i<5; i++) {
      this->fivePointLevels[i] = a.fivePointLevels[i];
      this->fivePointDurations[i] = a.fivePointDurations[i];
    }

    /*this->ahpdAttackDuration = a.ahpdAttackDuration;
    this->ahpdHoldDecayExponent = a.ahpdHoldDecayExponent;
    this->ahpdReleaseDecayExponent = a.ahpdReleaseDecayExponent;
    this->ahpdPartialDecayCoefficient = a.ahpdPartialDecayCoefficient;
    this->ahpdPartialDecayCoefficient2 = a.ahpdPartialDecayCoefficient2;*/

    this->padAttackDuration = a.padAttackDuration;
    this->padAttackPower = a.padAttackPower;
    this->padReleaseDuration = a.padReleaseDuration;
    this->padReleasePower = a.padReleasePower;

    this->envelopeGraph = a.envelopeGraph;
    this->envelopeSecondaryGraph = a.envelopeSecondaryGraph;
  }
  
  void setPreset(int index) {
    if(index >= 0 && index < envelopePresets.size()) {
      setPreset(envelopePresets[index]);
    }
  }

  void setPreset(const EnvelopePreset &envelopePreset) {
    type = envelopePreset.envelopeType;

    if(type == EnvelopeType::PowerAttackDecay) {
      padAttackDuration = envelopePreset.args.size() > 0 ? envelopePreset.args[0] : 0;
      padAttackPower = envelopePreset.args.size() > 1 ? envelopePreset.args[1] : 0;
      padReleaseDuration = envelopePreset.args.size() > 2 ? envelopePreset.args[2] : 0;
      padReleasePower = envelopePreset.args.size() > 3 ? envelopePreset.args[3] : 0;
    }
    if(type == EnvelopeType::AttackHoldExpDecay) {
      ahdAttackDuration = envelopePreset.args.size() > 0 ? envelopePreset.args[0] : 0;
      ahdHoldDecayExponent = envelopePreset.args.size() > 1 ? envelopePreset.args[1] : 0;
      ahdReleaseDecayExponent = envelopePreset.args.size() > 2 ? envelopePreset.args[2] : 0;
    }
    /*if(type == EnvelopeType::AttackHoldPartialDecays) {
      ahpdAttackDuration = envelopePreset.args.size() > 0 ? envelopePreset.args[0] : 0;
      ahpdHoldDecayExponent = envelopePreset.args.size() > 1 ? envelopePreset.args[1] : 0;
      ahpdReleaseDecayExponent = envelopePreset.args.size() > 2 ? envelopePreset.args[2] : 0;
      ahpdPartialDecayCoefficient = envelopePreset.args.size() > 3 ? envelopePreset.args[3] : 0;
      ahpdPartialDecayCoefficient2 = envelopePreset.args.size() > 4 ? envelopePreset.args[4] : 0;
    }*/
    if(type == EnvelopeType::AttackDecaySustainRelease) {
      adsrAttackDuration = envelopePreset.args.size() > 0 ? envelopePreset.args[0] : 0;
      adsrDecayDuration = envelopePreset.args.size() > 1 ? envelopePreset.args[1] : 0;
      adsrSustainLevel = envelopePreset.args.size() > 2 ? envelopePreset.args[2] : 0;
      adsrReleaseDuration = envelopePreset.args.size() > 3 ? envelopePreset.args[3] : 0;
    }
    if(type == EnvelopeType::FourPoints) {
      for(int i=0; i<4; i++) {
        fourPointLevels[i] = envelopePreset.args.size() > i ? envelopePreset.args[i] : 0;
        fourPointDurations[i] = envelopePreset.args.size() > i+4 ? envelopePreset.args[i+4] : 0;
      }
    }
    if(type == EnvelopeType::FivePoints) {
      for(int i=0; i<4; i++) {
        fivePointLevels[i] = envelopePreset.args.size() > i ? envelopePreset.args[i] : 0;
        fivePointDurations[i] = envelopePreset.args.size() > i+5 ? envelopePreset.args[i+5] : 0;
      }
    }
    prepare();
    updatePanel();
  }
  

  void update() {
    //printf("(debugging) at Envelope.update()...\n");
    prepare();
    //updatePanel();
  }

  void prepare() {
    updateAhd();
    updateAhd2();
    
    /*if(type == AttackHoldPartialDecays) {
      for(int i=0; i<PartialSet::maxNumPartials; i++) {
        ahpdPartialDecayFactors[i] =  1.0 / (1.0 + pow(i * ahpdPartialDecayCoefficient, ahpdPartialDecayCoefficient2));
      }
    }*/
    if(useLut) {
      prepareLookupBuffers();
    }
    updateEnvelopeGraph();
  }


  inline double getSample(double t, double keyHoldDuration, bool isKeyHolding, int partial = 0) {
    if(useLut && lutReady) return getSampleFromLookupBufferInterp(t, keyHoldDuration, isKeyHolding);

    return getSampleX(t, keyHoldDuration, isKeyHolding, partial);
  }

  


  inline double getSampleX(double t, double keyHoldDuration, bool isKeyHolding, int partial = 0) {
    if(t < 0) return 0.0;

    if(type == AttackHoldExpDecay) {
      if(t < ahdAttackDuration && isKeyHolding) {
        if(ahdAttackDuration == 0) return 1;
        return lerp(0.0, 1.0, t/ahdAttackDuration);
      }
      else {
        /*if(t >= ahdMaxLength) {
          return 0;
        }
        else */if(isKeyHolding) {
          return max(0.0, (exp(ahdHoldDecayExponent*(t-ahdAttackDuration)) + ahdOffset) / (1+ahdOffset));
        }
        else {
          if(keyHoldDuration < ahdAttackDuration) {
            double ampAfterAttack = ahdAttackDuration == 0 ? 1.0 : lerp(0.0, 1.0, keyHoldDuration/ahdAttackDuration);
            return max(0.0, ampAfterAttack * (exp(ahdReleaseDecayExponent*(t-keyHoldDuration)) + ahdOffset) / (1+ahdOffset));
          }
          else {
            //double ampAfterHold = (exp(ahdHoldDecayExponent*(t-ahdAttackDuration)) + ahdOffset) / (1+ahdOffset);
            double ampAfterHold = (exp(ahdHoldDecayExponent*(keyHoldDuration-ahdAttackDuration))) ;
            return max(0.0, (ampAfterHold * exp(ahdReleaseDecayExponent*(t-keyHoldDuration)) + ahdOffset) / (1+ahdOffset));
          }
        }
      }
    }
    else if(type == AttackHoldExpDecay2) {
      if(t < ahd2AttackDuration && isKeyHolding) {
        if(ahd2AttackDuration == 0) return 1;
        return lerp(0.0, 1.0, t/ahd2AttackDuration);
      }
      else {
        /*if(t >= ahd2MaxLength) {
          return 0;
        }
        else */if(isKeyHolding) {
          return max(0.0, (exp(ahd2HoldDecayExponent*pow(t-ahd2AttackDuration, ahd2HoldDecayPower)) + ahd2Offset) / (1+ahd2Offset));
        }
        else {
          if(keyHoldDuration < ahd2AttackDuration) {
            double ampAfterAttack = ahd2AttackDuration == 0 ? 1.0 : lerp(0.0, 1.0, keyHoldDuration/ahd2AttackDuration);
            return max(0.0, ampAfterAttack * (exp(ahd2ReleaseDecayExponent*(t-keyHoldDuration)) + ahd2Offset) / (1+ahd2Offset));
          }
          else {
            //double ampAfterHold = (exp(ahd2HoldDecayExponent*pow(t-ahd2AttackDuration, ahd2HoldDecayPower)) + ahd2Offset) / (1+ahd2Offset);
            double ampAfterHold = (exp(ahd2HoldDecayExponent*pow(keyHoldDuration-ahd2AttackDuration, ahd2HoldDecayPower)) ) ;
            return max(0.0, (ampAfterHold * exp(ahd2ReleaseDecayExponent*(t-keyHoldDuration)) + ahd2Offset) / (1+ahd2Offset));
          }
        }
      }
    }
    /*else if(type == AttackHoldPartialDecays) {
      if(t < 0.0) {
        return 0;
      }
      else if(t < ahpdAttackDuration) {
        if(ahpdAttackDuration == 0) return 1;
        return lerp(0.0, 1.0, t/ahpdAttackDuration);
        //return 0;
      }
      else {
        double t0 = ahpdAttackDuration;
        if(isKeyHolding) {
          return exp(ahpdHoldDecayExponent*(t-t0) * ahpdPartialDecayFactors[partial]);
        }
        else {
          double t1 = keyHoldDuration;
          double ampAfterHold = exp(ahpdHoldDecayExponent*(t-t0) * ahpdPartialDecayFactors[partial]);
          return ampAfterHold * exp(ahpdReleaseDecayExponent*(t-t1) * ahpdPartialDecayFactors[partial]);
        }
      }
    }*/
    else if(type == FourPoints) {
      double d2 = max(fourPointDurations[0] + fourPointDurations[1] + fourPointDurations[2], keyHoldDuration);

      if(t < 0.0) {
        return 0.0;
      }
      else if(t == 0.0) {
        return fourPointLevels[0];
      }
      if(fourPointDurations[0] > 0 && t < fourPointDurations[0]) {
        return lerp(fourPointLevels[0], fourPointLevels[1], t/fourPointDurations[0]);
      }
      else if(fourPointDurations[1] > 0 && t < fourPointDurations[0] + fourPointDurations[1]) {
        return lerp(fourPointLevels[1], fourPointLevels[2], (t-fourPointDurations[0])/fourPointDurations[1]);
      }
      /*if(fourPointDurations[0] > 0 && t < fourPointDurations[0] && isKeyHolding) {
        return lerp(fourPointLevels[0], fourPointLevels[1], t/fourPointDurations[0]);
      }
      else if(fourPointDurations[1] > 0 && t < fourPointDurations[0] + fourPointDurations[1] && isKeyHolding) {
        return lerp(fourPointLevels[1], fourPointLevels[2], (t-fourPointDurations[0])/fourPointDurations[1]);
      }
      else if(fourPointDurations[0] > keyHoldDuration && !isKeyHolding) {
        double ampAfterAttack = lerp(fourPointLevels[0], fourPointLevels[1], keyHoldDuration/fourPointDurations[0]);
        return fourPointDurations[3] == 0 ? fourPointLevels[3] : max(0.0, lerp(ampAfterAttack, fourPointLevels[3], (t-keyHoldDuration)/fourPointDurations[3]));
      }*/
      else if(isKeyHolding || t < fourPointDurations[0] + fourPointDurations[1] + fourPointDurations[2]) {
        return fourPointLevels[2];
      }
      else if(fourPointDurations[3] > 0 && t < d2 + fourPointDurations[3]) {
        return lerp(fourPointLevels[2], fourPointLevels[3], (t-d2)/fourPointDurations[3]);
      }
      else {
        return fourPointLevels[3];
      }
    }
    else if(type == FivePoints) {
      double tad = fivePointDurations[0] + fivePointDurations[1];
      double ht = isKeyHolding ? t-tad : keyHoldDuration-tad;
      double actualHoldDuration = max(min(fivePointDurations[3], ht), fivePointDurations[2]);

      if(t < 0.0) {
        return 0.0;
      }
      else if(t == 0.0) {
        return fivePointLevels[0];
      }
      if(fivePointDurations[0] > 0 && t < fivePointDurations[0] && isKeyHolding) {
        return lerp(fivePointLevels[0], fivePointLevels[1], t/fivePointDurations[0]);
      }
      else if(fivePointDurations[0] > keyHoldDuration && !isKeyHolding) {
        double ampAfterAttack = lerp(fivePointLevels[0], fivePointLevels[1], keyHoldDuration/fivePointDurations[0]);
        return fivePointDurations[4] == 0 ? fivePointLevels[4] : max(0.0, lerp(ampAfterAttack, fivePointLevels[4], (t-keyHoldDuration)/fivePointDurations[4]));
      }
      else if(fivePointDurations[1] > 0 && t < fivePointDurations[0] + fivePointDurations[1]) {
        return lerp(fivePointLevels[1], fivePointLevels[2], (t-fivePointDurations[0])/fivePointDurations[1]);
      }

      else if(fivePointDurations[3] > 0 && ((t < tad + fivePointDurations[2]) || (isKeyHolding && t < tad + fivePointDurations[3]))) {
        return lerp(fivePointLevels[2], fivePointLevels[3], (t-tad)/fivePointDurations[3]);
      }
      else if(fivePointDurations[4] > 0 && t < tad + actualHoldDuration + fivePointDurations[4]) {
        double levelAfterHold = fivePointDurations[3] == 0 ? fivePointLevels[3] : lerp(fivePointLevels[2], fivePointLevels[3], actualHoldDuration/fivePointDurations[3]);
        return lerp(levelAfterHold, fivePointLevels[4], (t-tad-actualHoldDuration)/fivePointDurations[4]);
      }
      else {
        return fivePointLevels[4];
      }
    }
    if(type == PowerAttackDecay) {
      if(t < 0.0 || t > padAttackDuration + padReleaseDuration) {
        return 0;
      }
      else if(t <= padAttackDuration && isKeyHolding) {
        if(padAttackDuration == 0) return 1;
        return pow(t/padAttackDuration, padAttackPower);
      }
      else {
        if(!isKeyHolding && keyHoldDuration < padAttackDuration) {
          double ampAfterAttack = padAttackDuration == 0 ? 1.0 : pow(keyHoldDuration/padAttackDuration, padAttackPower);
          return (padReleaseDuration == 0 || padReleasePower == 0) ? ampAfterAttack : ampAfterAttack * (1.0 - pow((t-keyHoldDuration)/padReleaseDuration, 1.0/padReleasePower));
        }
        else {
          return 1.0 - pow((t-padAttackDuration)/padReleaseDuration, 1.0/padReleasePower);
        }
      }
    }
    else {
      double t2 = max(adsrAttackDuration+adsrDecayDuration, keyHoldDuration);

      if(t < adsrAttackDuration && isKeyHolding) {
        if(adsrAttackDuration == 0) return 1;
        return t/adsrAttackDuration;
      }
      else if(keyHoldDuration < adsrAttackDuration && !isKeyHolding) {
        double ampAfterAttack = keyHoldDuration/adsrAttackDuration;
        return max(0.0, lerp(ampAfterAttack, 0.0, (t-keyHoldDuration)/adsrReleaseDuration));
      }
      else if(t < adsrAttackDuration + adsrDecayDuration) {
        if(adsrDecayDuration == 0) return adsrSustainLevel;
        return lerp(1.0, adsrSustainLevel, (t-adsrAttackDuration)/adsrDecayDuration);
      }
      else if(isKeyHolding) {
        return adsrSustainLevel;
      }
      else if(t - t2 < adsrReleaseDuration) {
        if(adsrReleaseDuration == 0) return 0;
        return lerp(adsrSustainLevel, 0, (t-t2)/adsrReleaseDuration);
      }
    }
    return 0.0;
  }


  double ahdMaxLength = 0;
  double ahd2MaxLength = 0;
  
  inline void updateAhd() {
    if(ahdOffset >= 0 || ahdHoldDecayExponent >= 0) ahdMaxLength = 1e10;
    else ahdMaxLength = ahdAttackDuration + log(-ahdOffset) / ahdHoldDecayExponent;
    //printf("max ahd t %f\n", ahdMaxLength);
  }
  
  inline double getAhdLength(double holdDuration = -1) {
    //printf("Getting ahd lenght...\n");
    if(ahdOffset >= 0 || ahdReleaseDecayExponent >= 0) {
      return 1e10;
    }
    else {
      updateAhd();
      if(ahdMaxLength <= holdDuration || holdDuration < 0 || ahdReleaseDecayExponent == ahdHoldDecayExponent) {
        return ahdMaxLength;
      }
      else {
        double levelAfterHold = 1;
        if(holdDuration < ahdAttackDuration) {
          levelAfterHold = lerp(0.0, 1.0, holdDuration/ahdAttackDuration);
        }
        else if(ahdHoldDecayExponent < 0 && holdDuration > ahdAttackDuration) {
          levelAfterHold = exp(ahdHoldDecayExponent*(holdDuration-ahdAttackDuration));
          //levelAfterHold = (exp(ahdHoldDecayExponent*(holdDuration-ahdAttackDuration)) + ahdOffset) / (1+ahdOffset);
        }
        double d = log(-ahdOffset/levelAfterHold) / ahdReleaseDecayExponent;
        return holdDuration + d;
      }
    }
  }
  
  inline void updateAhd2() {
    if(ahd2Offset >= 0 || ahd2HoldDecayExponent >= 0) ahd2MaxLength = 1e10;
    else ahd2MaxLength = ahd2AttackDuration + pow(log(-ahd2Offset) / ahd2HoldDecayExponent, 1.0/ahd2HoldDecayPower);
    //printf("max ahd t %f\n", ahdMaxLength);
  }
  
  inline double getAhd2Length(double holdDuration = -1) {
    //printf("Getting ahd lenght...\n");
    if(ahd2Offset >= 0 || ahd2ReleaseDecayExponent >= 0) {
      return 1e10;
    }
    else {
      updateAhd2();
      if(ahd2MaxLength <= holdDuration || holdDuration < 0 /*|| ahdReleaseDecayExponent == ahdHoldDecayExponent*/) {
        return ahd2MaxLength;
      }
      else {
        double levelAfterHold = 1;
        if(holdDuration < ahd2AttackDuration) {
          levelAfterHold = lerp(0.0, 1.0, holdDuration/ahd2AttackDuration);
        }
        else if(ahd2HoldDecayExponent < 0 && holdDuration > ahd2AttackDuration) {
          levelAfterHold = exp(ahd2HoldDecayExponent*pow(holdDuration-ahd2AttackDuration, ahd2HoldDecayPower));
          //levelAfterHold = (exp(ahd2HoldDecayExponent*pow(holdDuration-ahd2AttackDuration, ahd2HoldDecayPower)) + ahd2Offset) / (1+ahd2Offset);
        }
        double d = log(-ahd2Offset/levelAfterHold) / ahd2ReleaseDecayExponent;
        return holdDuration + d;
      }
    }
  }
  /*p * e^(a*t) + k = 0
  e^(a*t) = -k/p
  a*t = log(-k/p)
  t = log(-k/p) / a*/
  
  double getEnvelopeLength(double holdDuration = -1) {
    double envelopeLength = 0;
    double zeroLevel = 0.01;

    //if(holdDuration >= 0 && isHolding) {
      if(type == EnvelopeType::FourPoints) {
        for(int i=0; i<4; i++) {
          envelopeLength += fourPointDurations[i];
        }
      }
      else if(type == EnvelopeType::FivePoints) {
        // FIXME
        for(int i=0; i<5; i++) {
          envelopeLength += fivePointDurations[i];
        }
      }
      else if(type == EnvelopeType::AttackDecaySustainRelease) {
        if(adsrSustainLevel > 0) {
          if(holdDuration >= 0) {
            envelopeLength = adsrAttackDuration + adsrDecayDuration + holdDuration + adsrReleaseDuration;
          }
          else {
            envelopeLength = 1e10;
          }
        }
        else {
          envelopeLength = adsrAttackDuration + adsrDecayDuration;
        }
      }
      else if(type == EnvelopeType::AttackHoldExpDecay) {
        envelopeLength = getAhdLength(holdDuration);
      }
      else if(type == EnvelopeType::AttackHoldExpDecay2) {
        envelopeLength = getAhd2Length(holdDuration);
      }
      /*else if(type == EnvelopeType::AttackHoldPartialDecays) {
        // FIXME
        envelopeLength += ahpdAttackDuration;
        if(ahpdReleaseDecayExponent >= 0) {
          envelopeLength = 1e10;
        }
        else {
          envelopeLength += ahpdReleaseDecayExponent > -0.001 ? log(zeroLevel) / -0.001 : log(zeroLevel) / ahpdReleaseDecayExponent;
        }
      }*/
      else if(type == EnvelopeType::PowerAttackDecay) {
        envelopeLength += padAttackDuration;
        envelopeLength += padReleaseDuration;
      }
    //}
    /*else {
      if(type == EnvelopeType::FourPoints) {
        for(int i=0; i<4; i++) {
          envelopeLength += fourPointDurations[i];
        }
      }
      else if(type == EnvelopeType::FivePoints) {
        for(int i=0; i<5; i++) {
          envelopeLength += fivePointDurations[i];
        }
      }
      else if(type == EnvelopeType::AttackDecaySustainRelease) {
        envelopeLength += 1e10;
      }
      else if(type == EnvelopeType::AttackHoldExpDecay) {
        //envelopeLength += ahdAttackDuration;
        if(ahdReleaseDecayExponent >= 0) {
          envelopeLength = 10;
        }
        else {
          envelopeLength = ahdMaxLength;
          //envelopeLength += log(zeroLevel) / ahdReleaseDecayExponent;
        }
      }
      else if(type == EnvelopeType::AttackHoldPartialDecays) {
        envelopeLength += ahpdAttackDuration;
        if(ahpdReleaseDecayExponent >= 0) {
          envelopeLength = 10;
        }
        else {
          envelopeLength += ahpdReleaseDecayExponent > -0.001 ? log(zeroLevel) / -0.001 : log(zeroLevel) / ahpdReleaseDecayExponent;
        }
      }
      else if(type == EnvelopeType::PowerAttackDecay) {
        envelopeLength += padAttackDuration;
        envelopeLength += padReleaseDuration;
      }
    }*/
    return envelopeLength;
  }

  void getEnvelopeDimensions(double &envelopeLength, double &envelopeMaxGain) {
    envelopeLength = getEnvelopeLength();
    envelopeMaxGain = 0;

    if(type == EnvelopeType::FourPoints) {
      for(int i=0; i<4; i++) {
        envelopeMaxGain = max(envelopeMaxGain, fourPointLevels[i]);
      }
    }
    if(type == EnvelopeType::FivePoints) {
      for(int i=0; i<5; i++) {
        envelopeMaxGain = max(envelopeMaxGain, fivePointLevels[i]);
      }
    }
    if(type == EnvelopeType::AttackDecaySustainRelease) {
      envelopeMaxGain = 1;
      envelopeLength = getEnvelopeLength(1);
    }
    if(type == EnvelopeType::AttackHoldExpDecay) {
      envelopeMaxGain = 1;
    }
    if(type == EnvelopeType::AttackHoldExpDecay2) {
      envelopeMaxGain = 1;
    }
    /*if(type == EnvelopeType::AttackHoldPartialDecays) {
      envelopeLength += ahpdAttackDuration;
      envelopeMaxGain = 1;
    }*/
    if(type == EnvelopeType::PowerAttackDecay) {
      envelopeMaxGain = 1;
    }
  }



  void renderEnvelope(double envelopeLength, double envelopeMaxGain, double x, double y, double w, double h, double width, const Vec4d &color, const Vec4d &axisColor, GeomRenderer &geomRenderer, bool renderAxis = true) {
    int sx = int(w);
    geomRenderer.texture = NULL;
    geomRenderer.strokeWidth = 1.5;
    geomRenderer.strokeType = 1;
    geomRenderer.strokeColor = axisColor;

    if(renderAxis) {
      geomRenderer.drawLine(x, y, x+w, y);
      geomRenderer.drawLine(x, y, x, y+h);
      geomRenderer.drawLine(x+w, y+h, x+w, y);
      geomRenderer.drawLine(x+w, y+h, x, y+h);
      //geomRenderer.drawLine(x, y+0.5*h, x+w, y+0.5*h);

      for(int i=0; i<envelopeLength; i++) {
        geomRenderer.drawLine(x + w/envelopeLength * i, y+h+3, x + w/envelopeLength * i, y+h-3);
      }

      for(int i=0; i<envelopeMaxGain; i++) {
        geomRenderer.drawLine(x-3 , y+h-h/envelopeMaxGain*i, x+3, y+h-h/envelopeMaxGain*i);
      }
    }

    geomRenderer.strokeWidth = width;
    geomRenderer.strokeColor = color;

    double keyHoldTime = 0;

    if(type == EnvelopeType::AttackHoldExpDecay || type == EnvelopeType::PowerAttackDecay) {
      keyHoldTime = envelopeLength;
    }

    if(type == EnvelopeType::FivePoints ) {
      //keyHoldTime = fivePointDurations[3] - fivePointDurations[0] - fivePointDurations[1];
      keyHoldTime = fivePointDurations[0] + fivePointDurations[1] + fivePointDurations[3];
    }
    if(type == EnvelopeType::FourPoints ) {
      //keyHoldTime = fivePointDurations[3] - fivePointDurations[0] - fivePointDurations[1];
      //keyHoldTime = fourPointDurations[0] + fourPointDurations[1];
    }

    double ap = envelopeMaxGain == 0 ? 0 : getSample(0, keyHoldTime, keyHoldTime > 0) * h / envelopeMaxGain;
    double ap2 = -1;
    for(int i=1; i<sx; i++) {
      double t = (double)i/sx;
      //if(i == sx-1) t = 0;
      double a = envelopeMaxGain == 0 ? 0 : getSample(t*envelopeLength, keyHoldTime, t*envelopeLength < keyHoldTime) * h / envelopeMaxGain;
      geomRenderer.strokeColor.set(color);
      geomRenderer.drawLine(x+i-1, y+h-ap, x+i, y+h-a);
      ap = a;

      if(type == EnvelopeType::FivePoints && fivePointDurations[2] < fivePointDurations[3] && t*envelopeLength > fivePointDurations[0]+fivePointDurations[1]) {
        if(ap2 == -1) ap2 = a;
        double a2 = envelopeMaxGain == 0 ? 0 : getSample(t*envelopeLength, 0.0, false) * h / envelopeMaxGain;
        geomRenderer.strokeColor.set(color.x, color.y, color.z, color.w*0.5);
        geomRenderer.drawLine(x+i-1, y+h-ap2, x+i, y+h-a2);
        ap2 = a2;
      }
    }
  }

  void updateEnvelopeGraph() {
    if(!envelopeGraphPanel) return;
    double envelopeLength = 1, envelopeMaxGain = 1;
    getEnvelopeDimensions(envelopeLength, envelopeMaxGain);

    envelopeLength = min(max(envelopeLength, 4.0), 10);
    envelopeMaxGain = max(envelopeMaxGain, 1.0);

    double keyHoldTime = 0;

    if(type == EnvelopeType::AttackHoldExpDecay || type == EnvelopeType::AttackHoldExpDecay2 || type == EnvelopeType::PowerAttackDecay) {
      //keyHoldTime = envelopeLength;
      keyHoldTime = 2;
    }
    if(type == EnvelopeType::FivePoints) {
      keyHoldTime = fivePointDurations[0] + fivePointDurations[1] + fivePointDurations[3];
    }
    if(type == EnvelopeType::FourPoints) {
      keyHoldTime = fourPointDurations[0] + fourPointDurations[1];
    }
    if(type == EnvelopeType::AttackDecaySustainRelease) {
      keyHoldTime = adsrAttackDuration + adsrDecayDuration;
    }

    int n = 600;
    envelopeGraph.resize(n, 0);
    envelopeSecondaryGraph.resize(n, 0);

    for(int i=0; i<n; i++) {
      double t = (double)i/n;

      envelopeGraph[i] = getSample(t*envelopeLength, keyHoldTime, t*envelopeLength < keyHoldTime);

      if(type == EnvelopeType::FivePoints) {
        if(fivePointDurations[2] < fivePointDurations[3] && t*envelopeLength > fivePointDurations[0]+fivePointDurations[1]) {
          envelopeSecondaryGraph[i] = getSample(t*envelopeLength, 0.0, false);
        }
        else {
          envelopeSecondaryGraph[i] = envelopeGraph[i];
        }
      }
    }

    if(envelopeGraphPanel) {
      envelopeGraphPanel->numHorizontalMarks = envelopeLength <= 4 ? 5 : 0;
      envelopeGraphPanel->numVerticalMarks = (int)envelopeMaxGain+2;
      envelopeGraphPanel->horizontalAxisLimits.set(0, envelopeLength >= 10 ? (int) envelopeLength : envelopeLength);
      envelopeGraphPanel->verticalAxisLimits.set(0, envelopeMaxGain);
      envelopeGraphPanel->numHorizontalAxisLabels = 2;
      envelopeGraphPanel->horizontalAxisUnit = "s";
      envelopeGraphPanel->data = &envelopeGraph;
      envelopeGraphPanel->data2 = type == EnvelopeType::FivePoints ? &envelopeSecondaryGraph : NULL;
      envelopeGraphPanel->prerenderingNeeded = true;
    }
  }

  // FIXME new panel class etc.

  Panel *panel = NULL;
  SynthTitleBar *titleBar = NULL;
  ListBox *presetsGui = NULL;
  ListBox *envelopeTypeGui = NULL;
  
  NumberBox *ahdAttackDurationGui = NULL, *ahdHoldDecayExponentGui = NULL, *ahdReleaseDecayExponentGui = NULL, *ahdOffsetGui = NULL;
  
  NumberBox *ahd2AttackDurationGui = NULL, *ahd2HoldDecayPowerGui = NULL, *ahd2HoldDecayExponentGui = NULL, *ahd2ReleaseDecayExponentGui = NULL, *ahd2OffsetGui = NULL;

  NumberBox *adsrAttackDurationGui = NULL, *adsrDecayDurationGui = NULL, *adsrSustainLevelGui = NULL, *adsrReleaseDurationGui = NULL;

  /*NumberBox *ahpdAttackDurationGui = NULL, *ahpdHoldDecayExponentGui = NULL;
  NumberBox *ahpdPartialDecayCoefficientGui = NULL, *ahpdReleaseDecayExponentGui = NULL;
  NumberBox *ahpdPartialDecayCoefficient2Gui = NULL;*/

  NumberBox *padAttackDurationGui = NULL, *padAttackPowerGui = NULL;
  NumberBox *padReleaseDurationGui = NULL, *padReleasePowerGui = NULL;

  std::vector<NumberBox*> fourPointLevelsGui = std::vector<NumberBox*>(4, NULL);
  std::vector<NumberBox*> fourPointDurationsGui = std::vector<NumberBox*>(4, NULL);
  std::vector<NumberBox*> fivePointLevelsGui = std::vector<NumberBox*>(5, NULL);
  std::vector<NumberBox*> fivePointDurationsGui = std::vector<NumberBox*>(5, NULL);
  Label *durationLabel = NULL, *gainLabel = NULL;

  GraphPanel *envelopeGraphPanel = NULL;


  Panel *getPanel() {
    return panel;
  }

  Panel *addPanel(GuiElement *parentElement, const std::string &title = "") {

    double line = 0, lineHeight = 23;

    if(!panel) {
      panel = new Panel(title, 300, 590, 10, 10);
      panel->zLayer = 1;
      panel->addGuiEventListener(new EnvelopePanelListener(this));
      parentElement->addChildElement(panel);

      titleBar = new SynthTitleBar(title.size() > 0 ? title : "Envelope", panel);
      //titleBar->addPresetsKnob(presets);
      //line += titleBar->size.y + 5;

      ConstantWidthColumnPlacer layoutPlacer(250-20, 0, 10, titleBar->size.y+5);
      
      panel->addChildElement(envelopeTypeGui = new ListBox("Type", layoutPlacer, 9));
      envelopeTypeGui->addItems("AHE", "AHE2", "ADSR", "4-point", "5-point", "PAD");
      envelopeTypeGui->setValue(type);

      /*presetsGui = new ListBox("Preset", 10, line+=lineHeight, 12);
      for(int i=0; i<envelopePresets.size(); i++) {
        presetsGui->addItems(envelopePresets[i].name);
      }
      panel->addChildElement(presetsGui);*/

      envelopeGraphPanel = new GraphPanel(&envelopeGraph, false, 0, 0, 0, 1, 0, 1, 0, 0, "", "");
      envelopeGraphPanel->setSize(250, 85);
      updateEnvelopeGraph();
      panel->addChildElement(envelopeGraphPanel);
    }
    else {
      // FIXME
      //panel->deleteChildElement(envelopeTypeGui);
      panel->deleteChildElement(ahdAttackDurationGui);
      panel->deleteChildElement(ahdHoldDecayExponentGui);
      panel->deleteChildElement(ahdReleaseDecayExponentGui);
      panel->deleteChildElement(ahdOffsetGui);

      panel->deleteChildElement(ahd2AttackDurationGui);
      panel->deleteChildElement(ahd2HoldDecayExponentGui);
      panel->deleteChildElement(ahd2HoldDecayPowerGui);
      panel->deleteChildElement(ahd2ReleaseDecayExponentGui);
      panel->deleteChildElement(ahd2OffsetGui);
      
      panel->deleteChildElement(adsrAttackDurationGui);
      panel->deleteChildElement(adsrDecayDurationGui);
      panel->deleteChildElement(adsrSustainLevelGui);
      panel->deleteChildElement(adsrReleaseDurationGui);
      panel->deleteChildElement(durationLabel);
      panel->deleteChildElement(gainLabel);
      /*panel->deleteChildElement(ahpdAttackDurationGui);
      panel->deleteChildElement(ahpdHoldDecayExponentGui);
      panel->deleteChildElement(ahpdPartialDecayCoefficientGui);
      panel->deleteChildElement(ahpdPartialDecayCoefficient2Gui);
      panel->deleteChildElement(ahpdReleaseDecayExponentGui);*/
      panel->deleteChildElement(padAttackDurationGui);
      panel->deleteChildElement(padAttackPowerGui);
      panel->deleteChildElement(padReleasePowerGui);
      panel->deleteChildElement(padReleaseDurationGui);
      for(int i=0; i<4; i++) {
        panel->deleteChildElement(fourPointLevelsGui[i]);
        panel->deleteChildElement(fourPointDurationsGui[i]);
        fourPointLevelsGui[i] = NULL;
        fourPointDurationsGui[i] = NULL;
      }
      for(int i=0; i<5; i++) {
        panel->deleteChildElement(fivePointLevelsGui[i]);
        panel->deleteChildElement(fivePointDurationsGui[i]);
        fivePointLevelsGui[i] = NULL;
        fivePointDurationsGui[i] = NULL;
      }
      ahdAttackDurationGui = NULL;
      ahdHoldDecayExponentGui = NULL;
      ahdReleaseDecayExponentGui = NULL;
      ahdOffsetGui = NULL;

      ahd2AttackDurationGui = NULL;
      ahd2HoldDecayExponentGui = NULL;
      ahd2HoldDecayPowerGui = NULL;
      ahd2ReleaseDecayExponentGui = NULL;
      ahd2OffsetGui = NULL;
      
      adsrAttackDurationGui = NULL;
      adsrDecayDurationGui = NULL;
      adsrSustainLevelGui = NULL;
      adsrReleaseDurationGui = NULL;
      durationLabel = NULL;
      gainLabel = NULL;
      /*ahpdAttackDurationGui = NULL;
      ahpdHoldDecayExponentGui = NULL;
      ahpdPartialDecayCoefficientGui = NULL;
      ahpdPartialDecayCoefficient2Gui = NULL;
      ahpdReleaseDecayExponentGui = NULL;*/
      padAttackDurationGui = NULL;
      padAttackPowerGui = NULL;
      padReleaseDurationGui = NULL;
      padReleasePowerGui = NULL;

      //line += titleBar->size.y + 5;
    }

    ConstantWidthColumnPlacer layoutPlacer(250-20, 0, 10, envelopeTypeGui->pos.y + 23);


    if(type == EnvelopeType::PowerAttackDecay) {
      padAttackDurationGui = new NumberBox("attack duration", padAttackDuration, NumberBox::FLOATING_POINT, 0.0, 1e10, layoutPlacer, 10);
      padAttackPowerGui = new NumberBox("attack power", padAttackPower, NumberBox::FLOATING_POINT, 1e-10, 1e10, layoutPlacer, 10);
      padReleaseDurationGui = new NumberBox("release duration", padReleaseDuration, NumberBox::FLOATING_POINT, 0.0, 1e10, layoutPlacer, 10);
      padReleasePowerGui = new NumberBox("release power", padReleasePower, NumberBox::FLOATING_POINT, 1e-10, 1e10, layoutPlacer, 10);
      panel->addChildElement(padAttackDurationGui);
      panel->addChildElement(padAttackPowerGui);
      panel->addChildElement(padReleaseDurationGui);
      panel->addChildElement(padReleasePowerGui);
    }

    else if(type == EnvelopeType::AttackHoldExpDecay) {
      ahdAttackDurationGui = new NumberBox("attack duration", ahdAttackDuration, NumberBox::FLOATING_POINT, 0.0, 1e10, layoutPlacer, 10);
      ahdHoldDecayExponentGui = new NumberBox("hold decay exp.", ahdHoldDecayExponent, NumberBox::FLOATING_POINT, -1e10, 1e10, layoutPlacer, 10);
      ahdReleaseDecayExponentGui = new NumberBox("release decay exp.", ahdReleaseDecayExponent, NumberBox::FLOATING_POINT, -1e10, 1e10, layoutPlacer, 10);
      ahdOffsetGui = new NumberBox("offset", ahdOffset, NumberBox::FLOATING_POINT, -1e10, 0, layoutPlacer, 10);
      panel->addChildElement(ahdAttackDurationGui);
      panel->addChildElement(ahdHoldDecayExponentGui);
      panel->addChildElement(ahdReleaseDecayExponentGui);
      panel->addChildElement(ahdOffsetGui);
    }
    
    else if(type == EnvelopeType::AttackHoldExpDecay2) {
      ahd2AttackDurationGui = new NumberBox("attack duration", ahd2AttackDuration, NumberBox::FLOATING_POINT, 0.0, 1e10, layoutPlacer, 10);
      ahd2HoldDecayExponentGui= new NumberBox("hold decay exp.", ahd2HoldDecayExponent, NumberBox::FLOATING_POINT, -1e10, 1e10, layoutPlacer, 10);
      ahd2HoldDecayPowerGui = new NumberBox("hold decay power", ahd2HoldDecayPower, NumberBox::FLOATING_POINT, 1e-2, 1, layoutPlacer, 10);
      ahd2ReleaseDecayExponentGui = new NumberBox("release decay exp.", ahd2ReleaseDecayExponent, NumberBox::FLOATING_POINT, -1e10, 1e10, layoutPlacer, 10);
      ahd2OffsetGui = new NumberBox("offset", ahd2Offset, NumberBox::FLOATING_POINT, -1e10, 0, layoutPlacer, 10);
      panel->addChildElement(ahd2AttackDurationGui);
      panel->addChildElement(ahd2HoldDecayExponentGui);
      panel->addChildElement(ahd2HoldDecayPowerGui);
      panel->addChildElement(ahd2ReleaseDecayExponentGui);
      panel->addChildElement(ahd2OffsetGui);
    }
    
    /*else if(type == EnvelopeType::AttackHoldPartialDecays) {
      ahpdAttackDurationGui = new NumberBox("attack duration", ahpdAttackDuration, NumberBox::FLOATING_POINT, 0.0, 1e10, layoutPlacer, 10);
      ahpdHoldDecayExponentGui = new NumberBox("hold decay exp.", ahpdHoldDecayExponent, NumberBox::FLOATING_POINT, -1e10, 1e10, layoutPlacer, 10);
      ahpdReleaseDecayExponentGui = new NumberBox("release decay exp.", ahpdReleaseDecayExponent, NumberBox::FLOATING_POINT, -1e10, 1e10, layoutPlacer, 10);
      ahpdPartialDecayCoefficientGui = new NumberBox("coefficient lin.", ahpdPartialDecayCoefficient, NumberBox::FLOATING_POINT, 0.0, 1e10, layoutPlacer, 8);
      ahpdPartialDecayCoefficient2Gui = new NumberBox("coefficient pow.", ahpdPartialDecayCoefficient2, NumberBox::FLOATING_POINT, 1e-10, 1, layoutPlacer, 7);
      panel->addChildElement(ahpdAttackDurationGui);
      panel->addChildElement(ahpdHoldDecayExponentGui);
      panel->addChildElement(ahpdReleaseDecayExponentGui);
      panel->addChildElement(ahpdPartialDecayCoefficientGui);
      panel->addChildElement(ahpdPartialDecayCoefficient2Gui);
    }*/
    else if(type == EnvelopeType::AttackDecaySustainRelease) {
      adsrAttackDurationGui = new NumberBox("attack duration", adsrAttackDuration, NumberBox::FLOATING_POINT, 0.0, 100, layoutPlacer, 10);
      adsrDecayDurationGui = new NumberBox("decay duration", adsrDecayDuration, NumberBox::FLOATING_POINT, 0.0, 100, layoutPlacer, 10);
      adsrSustainLevelGui = new NumberBox("sustain level", adsrSustainLevel, NumberBox::FLOATING_POINT, 0.0, 1.0, layoutPlacer, 10);
      adsrReleaseDurationGui = new NumberBox("release duration", adsrReleaseDuration, NumberBox::FLOATING_POINT, 0.0, 100, layoutPlacer, 10);
      panel->addChildElement(adsrAttackDurationGui);
      panel->addChildElement(adsrDecayDurationGui);
      panel->addChildElement(adsrSustainLevelGui);
      panel->addChildElement(adsrReleaseDurationGui);
    }
    else if(type == EnvelopeType::FourPoints) {
      ConstantWidthColumnPlacer layoutPlacerLeft(250/2-10-10, 0, 10, layoutPlacer.getY());
      ConstantWidthColumnPlacer layoutPlacerRight(250/2-10-10, 0, 250/2+5, layoutPlacer.getY());
      
      durationLabel = new Label("Duration", layoutPlacerLeft);
      gainLabel = new Label("Gain", layoutPlacerRight);
      panel->addChildElement(durationLabel);
      panel->addChildElement(gainLabel);
      for(int i=0; i<4; i++) {
        fourPointDurationsGui[i] = new NumberBox(""+std::to_string(i+1), fourPointDurations[i], NumberBox::FLOATING_POINT, 0.0, 1e10, layoutPlacerLeft, 9);
        fourPointLevelsGui[i] = new NumberBox(""+std::to_string(i+1), fourPointLevels[i], NumberBox::FLOATING_POINT, 0.0, 1e10, layoutPlacerRight, 9);
        panel->addChildElement(fourPointLevelsGui[i]);
        panel->addChildElement(fourPointDurationsGui[i]);
      }
      layoutPlacer.y = layoutPlacerLeft.y;
    }
    else if(type == EnvelopeType::FivePoints) {
      ConstantWidthColumnPlacer layoutPlacerLeft(250/2-10-10, 0, 10, layoutPlacer.getY());
      ConstantWidthColumnPlacer layoutPlacerRight(250/2-10-10, 0, 250/2+5, layoutPlacer.getY());
      
      durationLabel = new Label("Duration", layoutPlacerLeft);
      gainLabel = new Label("Gain", layoutPlacerRight);
      panel->addChildElement(durationLabel);
      panel->addChildElement(gainLabel);
      for(int i=0; i<5; i++) {
        fivePointDurationsGui[i] = new NumberBox(""+std::to_string(i+1), fivePointDurations[i], NumberBox::FLOATING_POINT, 0.0, 1e10, layoutPlacerLeft, 9);
        fivePointLevelsGui[i] = new NumberBox(""+std::to_string(i+1), fivePointLevels[i], NumberBox::FLOATING_POINT, 0.0, 1e10, layoutPlacerRight, 9);
        panel->addChildElement(fivePointDurationsGui[i]);
        panel->addChildElement(fivePointLevelsGui[i]);
      }
      layoutPlacer.y = layoutPlacerLeft.y;
    }

    panel->setSize(250, layoutPlacer.getY() + 10);

    //setOscillatorSubPanelPositions();
    if(envelopeGraphPanel) {
      envelopeGraphPanel->setPosition(0, panel->size.y + 5);
    }

    return panel;
  }

  void updatePanel() {
    if(panel) {
      addPanel(NULL);
    }
  }

  void removePanel(GuiElement *parentElement) {
    if(!panel) return;
    panel->deleteChildElement(envelopeGraphPanel);
    envelopeGraphPanel = NULL;
    parentElement->deleteChildElement(panel);
    panel = NULL;
    envelopeTypeGui = NULL;
    ahdAttackDurationGui = NULL;
    ahdHoldDecayExponentGui = NULL;
    ahdReleaseDecayExponentGui = NULL;
    ahdOffsetGui = NULL;
    
    ahd2AttackDurationGui = NULL;
    ahd2HoldDecayExponentGui = NULL;
    ahd2HoldDecayPowerGui = NULL;
    ahd2ReleaseDecayExponentGui = NULL;
    ahd2OffsetGui = NULL;

    adsrAttackDurationGui = NULL;
    adsrDecayDurationGui = NULL;
    adsrSustainLevelGui = NULL;
    adsrReleaseDurationGui = NULL;
    /*ahpdAttackDurationGui = NULL;
    ahpdHoldDecayExponentGui = NULL;
    ahpdPartialDecayCoefficientGui = NULL;
    ahpdPartialDecayCoefficient2Gui = NULL;
    ahpdReleaseDecayExponentGui = NULL;*/
    padAttackDurationGui = NULL;
    padAttackPowerGui = NULL;
    padReleaseDurationGui = NULL;
    padReleasePowerGui = NULL;
    for(int i=0; i<4; i++) {
      fourPointLevelsGui[i] = NULL;
      fourPointDurationsGui[i] = NULL;
    }
    for(int i=0; i<5; i++) {
      fivePointLevelsGui[i] = NULL;
      fivePointDurationsGui[i] = NULL;
    }
  }


  struct EnvelopePanelListener : public GuiEventListener {
    Envelope *envelope;
    EnvelopePanelListener(Envelope *envelope) {
      this->envelope = envelope;
    }
    void onAction(GuiElement *guiElement) {
      if(guiElement == envelope->presetsGui) {
        int value;
        guiElement->getValue((void*)&value);
        envelope->setPreset(value);
      }
    }
    void onValueChange(GuiElement *guiElement) {
      if(guiElement == envelope->envelopeTypeGui) {
        guiElement->getValue((void*)&envelope->type);
        //envelope->removepanel();
        envelope->updatePanel();
      }

      if(guiElement == envelope->padAttackDurationGui) {
        guiElement->getValue((void*)&envelope->padAttackDuration);
      }
      if(guiElement == envelope->padAttackPowerGui) {
        guiElement->getValue((void*)&envelope->padAttackPower);
      }
      if(guiElement == envelope->padReleaseDurationGui) {
        guiElement->getValue((void*)&envelope->padReleaseDuration);
      }
      if(guiElement == envelope->padReleasePowerGui) {
        guiElement->getValue((void*)&envelope->padReleasePower);
      }


      /*if(guiElement == envelope->ahpdAttackDurationGui) {
        guiElement->getValue((void*)&envelope->ahpdAttackDuration);
      }
      if(guiElement == envelope->ahpdHoldDecayExponentGui) {
        guiElement->getValue((void*)&envelope->ahpdHoldDecayExponent);
        if(envelope->ahpdHoldDecayExponent < envelope->ahpdReleaseDecayExponent) {
          envelope->ahpdReleaseDecayExponent = envelope->ahpdHoldDecayExponent;
          envelope->ahpdReleaseDecayExponentGui->setValue(envelope->ahpdReleaseDecayExponent);
        }
      }
      if(guiElement == envelope->ahpdReleaseDecayExponentGui) {
        guiElement->getValue((void*)&envelope->ahpdReleaseDecayExponent);
        if(envelope->ahpdHoldDecayExponent < envelope->ahpdReleaseDecayExponent) {
          envelope->ahpdHoldDecayExponent = envelope->ahpdReleaseDecayExponent;
          envelope->ahpdHoldDecayExponentGui->setValue(envelope->ahpdHoldDecayExponent);
        }
      }
      if(guiElement == envelope->ahpdPartialDecayCoefficientGui) {
        guiElement->getValue((void*)&envelope->ahpdPartialDecayCoefficient);
      }
      if(guiElement == envelope->ahpdPartialDecayCoefficient2Gui) {
        guiElement->getValue((void*)&envelope->ahpdPartialDecayCoefficient2);
      }*/

      if(guiElement == envelope->ahdAttackDurationGui) {
        guiElement->getValue((void*)&envelope->ahdAttackDuration);
      }
      if(guiElement == envelope->ahdOffsetGui) {
        guiElement->getValue((void*)&envelope->ahdOffset);
      }
      if(guiElement == envelope->ahdHoldDecayExponentGui) {
        guiElement->getValue((void*)&envelope->ahdHoldDecayExponent);
        if(envelope->ahdHoldDecayExponent < envelope->ahdReleaseDecayExponent) {
          envelope->ahdReleaseDecayExponent = envelope->ahdHoldDecayExponent;
          envelope->ahdReleaseDecayExponentGui->setValue(envelope->ahdReleaseDecayExponent);
        }
      }
      if(guiElement == envelope->ahdReleaseDecayExponentGui) {
        guiElement->getValue((void*)&envelope->ahdReleaseDecayExponent);
        if(envelope->ahdHoldDecayExponent < envelope->ahdReleaseDecayExponent) {
          envelope->ahdHoldDecayExponent = envelope->ahdReleaseDecayExponent;
          envelope->ahdHoldDecayExponentGui->setValue(envelope->ahdHoldDecayExponent);
        }
      }
      
      if(guiElement == envelope->ahd2AttackDurationGui) {
        guiElement->getValue((void*)&envelope->ahd2AttackDuration);
      }
      if(guiElement == envelope->ahd2OffsetGui) {
        guiElement->getValue((void*)&envelope->ahd2Offset);
      }
      if(guiElement == envelope->ahd2HoldDecayPowerGui) {
        guiElement->getValue((void*)&envelope->ahd2HoldDecayPower);
      }
      if(guiElement == envelope->ahd2HoldDecayExponentGui) {
        guiElement->getValue((void*)&envelope->ahd2HoldDecayExponent);
        if(envelope->ahd2HoldDecayExponent < envelope->ahd2ReleaseDecayExponent) {
          envelope->ahd2ReleaseDecayExponent = envelope->ahd2HoldDecayExponent;
          envelope->ahd2ReleaseDecayExponentGui->setValue(envelope->ahd2ReleaseDecayExponent);
        }
      }
      if(guiElement == envelope->ahd2ReleaseDecayExponentGui) {
        guiElement->getValue((void*)&envelope->ahd2ReleaseDecayExponent);
        if(envelope->ahd2HoldDecayExponent < envelope->ahd2ReleaseDecayExponent) {
          envelope->ahd2HoldDecayExponent = envelope->ahd2ReleaseDecayExponent;
          envelope->ahd2HoldDecayExponentGui->setValue(envelope->ahd2HoldDecayExponent);
        }
      }
            
      
      if(guiElement == envelope->adsrAttackDurationGui) {
        guiElement->getValue((void*)&envelope->adsrAttackDuration);
      }
      if(guiElement == envelope->adsrDecayDurationGui) {
        guiElement->getValue((void*)&envelope->adsrDecayDuration);
      }
      if(guiElement == envelope->adsrSustainLevelGui) {
        guiElement->getValue((void*)&envelope->adsrSustainLevel);
      }
      if(guiElement == envelope->adsrReleaseDurationGui) {
        guiElement->getValue((void*)&envelope->adsrReleaseDuration);
      }
      for(int i=0; i<4; i++) {
        if(guiElement == envelope->fourPointDurationsGui[i]) {
          guiElement->getValue((void*)&envelope->fourPointDurations[i]);
        }
        if(guiElement == envelope->fourPointLevelsGui[i]) {
          guiElement->getValue((void*)&envelope->fourPointLevels[i]);
        }
      }
      for(int i=0; i<5; i++) {
        if(guiElement == envelope->fivePointDurationsGui[i]) {
          guiElement->getValue((void*)&envelope->fivePointDurations[i]);
          if(guiElement == envelope->fivePointDurationsGui[2] && envelope->fivePointDurations[2] > envelope->fivePointDurations[3]) {
            envelope->fivePointDurations[3] = envelope->fivePointDurations[2];
            envelope->fivePointDurationsGui[3]->setValue(envelope->fivePointDurations[3]);
          }
          if(guiElement == envelope->fivePointDurationsGui[3] && envelope->fivePointDurations[2] > envelope->fivePointDurations[3]) {
            envelope->fivePointDurations[2] = envelope->fivePointDurations[3];
            envelope->fivePointDurationsGui[2]->setValue(envelope->fivePointDurations[2]);
          }
        }
        if(guiElement == envelope->fivePointLevelsGui[i]) {
          guiElement->getValue((void*)&envelope->fivePointLevels[i]);
        }
      }
      envelope->prepare();
    }
  };




  bool useLut = false, lutReady = false;
  int lookupBufferWidth = 1<<9;
  std::vector<double> lookupBuffer;
  std::vector<double> lookupBufferKeyHolding;
  double lookupBufferLengthInSeconds;

  inline double getSampleFromLookupBuffer(double t, double keyHoldDuration, bool isKeyHolding) {
    if(t < 0 || t >= lookupBufferLengthInSeconds) return 0;

    int i = lookupBufferLengthInSeconds == 0 ? 0 : int(t / lookupBufferLengthInSeconds * lookupBufferWidth);
    int j = lookupBufferLengthInSeconds == 0 ? 0 : int(keyHoldDuration / lookupBufferLengthInSeconds * lookupBufferWidth);

    i = min(i, lookupBufferWidth-1);
    j = min(j, lookupBufferWidth-1);

    if(isKeyHolding) {
      return lookupBufferKeyHolding[i+lookupBufferWidth*j];
    }
    else {
      return lookupBuffer[i+lookupBufferWidth*j];
    }
  }
  
  inline double getSampleFromLookupBufferInterp(double t, double keyHoldDuration, bool isKeyHolding) {
    if(t < 0 || t >= lookupBufferLengthInSeconds*0.98) return 0;

    double f = lookupBufferLengthInSeconds == 0 ? 0 : t / lookupBufferLengthInSeconds * lookupBufferWidth;
    int i0 = int(f);
    int i1 = i0 + 1;
    double q = f - i0;

    double f2 = lookupBufferLengthInSeconds == 0 ? 0 : keyHoldDuration / lookupBufferLengthInSeconds * lookupBufferWidth;
    int j0 = int(f2);
    int j1 = j0 + 1;
    double q2 = f2 - j0;

    if(i1 >= lookupBufferWidth || j1 >= lookupBufferWidth) return 0;

    if(isKeyHolding) {
      double w0 = (1.0-q) * lookupBufferKeyHolding[i0+lookupBufferWidth*j0] + q * lookupBufferKeyHolding[i1+lookupBufferWidth*j0];
      double w1 = (1.0-q) * lookupBufferKeyHolding[i0+lookupBufferWidth*j1] + q * lookupBufferKeyHolding[i1+lookupBufferWidth*j1];
      return (1.0-q2) * w0 + q2 * w1;
    }
    else {
      double w0 = (1.0-q) * lookupBuffer[i0+lookupBufferWidth*j0] + q * lookupBuffer[i1+lookupBufferWidth*j0];
      double w1 = (1.0-q) * lookupBuffer[i0+lookupBufferWidth*j1] + q * lookupBuffer[i1+lookupBufferWidth*j1];
      return (1.0-q2) * w0 + q2 * w1;
    }
  }

  void prepareLookupBuffers() {
    lutReady = false;
    double maxGain;
    getEnvelopeDimensions(lookupBufferLengthInSeconds, maxGain);
    //printf("env length %f\n", lookupBufferLengthInSeconds);

    lookupBuffer.resize(lookupBufferWidth*lookupBufferWidth);
    lookupBufferKeyHolding.resize(lookupBufferWidth*lookupBufferWidth);

    for(int i=0; i<lookupBufferWidth; i++) {
      double t = map(i, 0, lookupBufferWidth-1, 0.0, lookupBufferLengthInSeconds);
      for(int j=0; j<lookupBufferWidth; j++) {
        double keyHoldDuration = map(j, 0, lookupBufferWidth-1, 0.0, lookupBufferLengthInSeconds);
        lookupBuffer[i+lookupBufferWidth*j] = getSampleX(t, keyHoldDuration, false);
        lookupBufferKeyHolding[i+lookupBufferWidth*j] = getSampleX(t, 0, true);
        //if(i < 10 && j < 10) printf("%f s, %f s, %f\n", t, keyHoldDuration, lookupBufferKeyHolding[i+lookupBufferWidth*j]);
      }
    }
    lutReady = true;
  }


  static std::string getClassName() {
    return "envelope";
  }
  
  virtual std::string getBlockName() {
    return getClassName();
  }


  virtual void decodeParameters() {
    std::string typeName;
    getStringParameter("type", typeName);
    for(int i=0; i<envelopeTypeShortNames.size(); i++) {
      if(typeName == envelopeTypeShortNames[i]) {
        type = (EnvelopeType)i;
        break;
      }
    }
    
    if(type == EnvelopeType::AttackHoldExpDecay) {
      getNumericParameter("parameters", ahdAttackDuration, 0);
      getNumericParameter("parameters", ahdHoldDecayExponent, 1);
      getNumericParameter("parameters", ahdReleaseDecayExponent, 2);
      getNumericParameter("parameters", ahdOffset, 3);
    }
    else if(type == EnvelopeType::AttackHoldExpDecay2) {
      getNumericParameter("parameters", ahd2AttackDuration, 0);
      getNumericParameter("parameters", ahd2HoldDecayExponent, 1);
      getNumericParameter("parameters", ahd2HoldDecayPower, 2);
      getNumericParameter("parameters", ahd2ReleaseDecayExponent, 3);
      getNumericParameter("parameters", ahd2Offset, 4);
    }
    else if(type == EnvelopeType::AttackDecaySustainRelease) {
      getNumericParameter("parameters", adsrAttackDuration, 0);
      getNumericParameter("parameters", adsrDecayDuration, 1);
      getNumericParameter("parameters", adsrSustainLevel, 2);
      getNumericParameter("parameters", adsrReleaseDuration, 3);
    }
    else if(type == EnvelopeType::FourPoints) {
      for(int i=0; i<4; i++) {
        getNumericParameter("parameters", fourPointLevels[i], i);
        getNumericParameter("parameters", fourPointDurations[i], 4+i);
      }
    }
    else if(type == EnvelopeType::FivePoints) {
      for(int i=0; i<5; i++) {
        getNumericParameter("parameters", fivePointLevels[i], i);
        getNumericParameter("parameters", fivePointDurations[i], 5+i);
      }
    }
    /*else if(type == EnvelopeType::AttackHoldPartialDecays) {
      getNumericParameter("parameters", ahpdAttackDuration, 0);
      getNumericParameter("parameters", ahpdHoldDecayExponent, 1);
      getNumericParameter("parameters", ahpdReleaseDecayExponent, 2);
      getNumericParameter("parameters", ahpdPartialDecayCoefficient, 3);
      getNumericParameter("parameters", ahpdPartialDecayCoefficient2, 4);
    }*/
    else if(type == EnvelopeType::PowerAttackDecay) {
      getNumericParameter("parameters", padAttackDuration, 0);
      getNumericParameter("parameters", padAttackPower, 1);
      getNumericParameter("parameters", padReleaseDuration, 2);
      getNumericParameter("parameters", padReleasePower, 3);
    }
    prepare();
    updatePanel();
  }
  
  virtual void encodeParameters() {
    clearParameters();
    putStringParameter("type", envelopeTypeShortNames[type]);
    
    if(type == EnvelopeType::AttackHoldExpDecay) {
      putNumericParameter("parameters", {ahdAttackDuration, ahdHoldDecayExponent, ahdReleaseDecayExponent, ahdOffset});
    }
    else if(type == EnvelopeType::AttackHoldExpDecay2) {
      putNumericParameter("parameters", {ahd2AttackDuration, ahd2HoldDecayExponent, ahd2HoldDecayPower, ahd2ReleaseDecayExponent, ahd2Offset});
    }
    else if(type == EnvelopeType::AttackDecaySustainRelease) {
      putNumericParameter("parameters", {adsrAttackDuration, adsrDecayDuration, adsrSustainLevel, adsrReleaseDuration});
    }
    else if(type == EnvelopeType::FourPoints) {
      doubleParameters.push_back(DoubleParameter("parameters"));
      for(int i=0; i<4; i++) {
        doubleParameters[doubleParameters.size()-1].values.push_back(fourPointLevels[i]);
      }
      for(int i=0; i<4; i++) {
        doubleParameters[doubleParameters.size()-1].values.push_back(fourPointDurations[i]);
      }
    }
    else if(type == EnvelopeType::FivePoints) {
      doubleParameters.push_back(DoubleParameter("parameters"));
      for(int i=0; i<5; i++) {
        doubleParameters[doubleParameters.size()-1].values.push_back(fivePointLevels[i]);
      }
      for(int i=0; i<5; i++) {
        doubleParameters[doubleParameters.size()-1].values.push_back(fivePointDurations[i]);
      }
    }
    /*else if(type == EnvelopeType::AttackHoldPartialDecays) {
      putNumericParameter("parameters", {ahpdAttackDuration, ahpdHoldDecayExponent, ahpdReleaseDecayExponent, ahpdPartialDecayCoefficient, ahpdPartialDecayCoefficient2});
    }*/
    else if(type == EnvelopeType::PowerAttackDecay) {
      putNumericParameter("parameters", {padAttackDuration, padAttackPower, padReleaseDuration, padReleasePower});
    }
  }











};
