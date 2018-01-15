//
// Created by Arseny Tolmachev on 2017/03/11.
//

#ifndef JUMANPP_JUMANPP_ARGS_H
#define JUMANPP_JUMANPP_ARGS_H

#include <ostream>
#include <string>
#include "core/analysis/rnn_scorer.h"
#include "core_config.h"
#include "util/cfg.h"
#include "util/status.hpp"

namespace jumanpp {
namespace jumandic {

enum class OutputType {
  Version,
  ModelInfo,
  Segmentation,
  Juman,
  Morph,
  FullMorph,
  DicSubset,
  Lattice,
#if defined(JPP_ENABLE_DEV_TOOLS)
  GlobalBeamPos,
#if defined(JPP_USE_PROTOBUF)
  FullLatticeDump
#endif
#endif
};

struct JumanppConf {
  util::Cfg<std::string> configFile;
  util::Cfg<std::string> modelFile;
  util::Cfg<OutputType> outputType = OutputType::Juman;
  util::Cfg<std::string> inputFile{"-"};
  util::Cfg<std::string> rnnModelFile;
  core::analysis::rnn::RnnInferenceConfig rnnConfig{};
  util::Cfg<std::string> graphvizDir;
  util::Cfg<i32> beamSize = 5;
  util::Cfg<i32> beamOutput = 1;
  util::Cfg<i32> globalBeam = 6;
  util::Cfg<i32> rightBeam = 5;
  util::Cfg<i32> rightCheck = 1;
  util::Cfg<i32> logLevel = 0;
  util::Cfg<i32> autoStep = 0;
  util::Cfg<std::string> segmentSeparator{" "};

  void mergeWith(const JumanppConf& o) {
    configFile.mergeWith(o.configFile);
    modelFile.mergeWith(o.modelFile);
    outputType.mergeWith(o.outputType);
    inputFile.mergeWith(o.inputFile);
    rnnModelFile.mergeWith(o.rnnModelFile);
    rnnConfig.mergeWith(o.rnnConfig);
    graphvizDir.mergeWith(o.graphvizDir);
    beamSize.mergeWith(o.beamSize);
    beamOutput.mergeWith(o.beamOutput);
    globalBeam.mergeWith(o.globalBeam);
    rightBeam.mergeWith(o.rightBeam);
    rightCheck.mergeWith(o.rightCheck);
    logLevel.mergeWith(o.logLevel);
    autoStep.mergeWith(o.autoStep);
    segmentSeparator.mergeWith(o.segmentSeparator);
  }

  friend std::ostream& operator<<(std::ostream& os, const JumanppConf& conf) {
    os << "\nconfigFile: " << conf.configFile
       << "\nmodelFile: " << conf.modelFile
       << "\noutputType: " << static_cast<int>(conf.outputType.value())
       << "\ninputFile: " << conf.inputFile
       << "\nrnnModelFile: " << conf.rnnModelFile
       << "\nrnnConfig: " << conf.rnnConfig
       << "\ngraphvizDir: " << conf.graphvizDir
       << "\nbeamSize: " << conf.beamSize << "\nbeamOutput: " << conf.beamOutput
       << "\nglobalBeam: " << conf.globalBeam
       << "\nrightBeam: " << conf.rightBeam
       << "\nrightCheck: " << conf.rightCheck
       << "\nsegmentSeparator: " << conf.segmentSeparator
       << "\nautoStep: " << conf.autoStep << "\nlogLevel: " << conf.logLevel;
    return os;
  }
};

Status parseArgs(int argc, const char* argv[], JumanppConf* result);

}  // namespace jumandic
}  // namespace jumanpp

#endif  // JUMANPP_JUMANPP_ARGS_H
