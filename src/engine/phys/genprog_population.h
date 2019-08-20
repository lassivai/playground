#pragma once
#include "simulation.h"

struct GenProgPopulation
{
  std::vector<GenProgComposite*> genProgCompositePopulation;
  std::vector<double> normalizedTotalScores;
  double normalizedTotalScoreSum;
  int n;
  GenProgComposite *originalGenProgComposite = NULL;

  Simulation *simulation = NULL;



  void initPopulation(Simulation *simulation, GenProgComposite *originalGenProgComposite, int n, bool randomize) {
    this->simulation = simulation;
    this->originalGenProgComposite = originalGenProgComposite;

    if(genProgCompositePopulation.size() > 0) {
      for(int i=0; i<genProgCompositePopulation.size(); i++) {
        simulation->removeComposite(genProgCompositePopulation[i]);
      }
      genProgCompositePopulation.clear();
    }

    for(int i=0; i<n; i++) {
      GenProgComposite *child = simulation->replicateComposite(originalGenProgComposite);
      if(randomize) {
        child->randomize();
      }
      else {
        child->mutate();
      }

      genProgCompositePopulation.push_back(child);
    }
    this->n = n;
  }

  void normalizeScores() {
    if(genProgCompositePopulation.size() == 0) return;

    std::vector<double> maxScores(genProgCompositePopulation[0]->genProgScorings.size(), -1e100);
    std::vector<double> minScores(genProgCompositePopulation[0]->genProgScorings.size(), 1e100);

    normalizedTotalScores = std::vector<double>(genProgCompositePopulation.size(), 0.0);

    for(int k=0; k<genProgCompositePopulation[0]->genProgScorings.size(); k++) {
      for(int i=0; i<genProgCompositePopulation.size(); i++) {
        minScores[k] = min(minScores[k], genProgCompositePopulation[i]->genProgScorings[k]->score);
        maxScores[k] = max(maxScores[k], genProgCompositePopulation[i]->genProgScorings[k]->score);
      }
    }

    normalizedTotalScoreSum = 0;

    // FIXME this thing isn't entirely fair, some irrelevant genes might give unreasonably high scores,
    //       if weights are not set properly manually
    for(int k=0; k<genProgCompositePopulation[0]->genProgScorings.size(); k++) {
      for(int i=0; i<genProgCompositePopulation.size(); i++) {
        double nq = map(genProgCompositePopulation[i]->genProgScorings[k]->score, minScores[k], maxScores[k], 0.0, 1.0);
        genProgCompositePopulation[i]->genProgScorings[k]->normalizedScore = nq * genProgCompositePopulation[i]->genProgScorings[k]->scoreWeight;
        normalizedTotalScores[i] += genProgCompositePopulation[i]->genProgScorings[k]->normalizedScore;
        normalizedTotalScoreSum += normalizedTotalScores[i];
      }
    }
  }


  void createNewGeneration(int nNew) {
    std::vector<GenProgComposite*> genProgCompositePopulationNextGeneration;

    for(int i=0; i<n; i++) {
      int nIndividualsX = (int)round(nNew * normalizedTotalScores[i] / normalizedTotalScoreSum);
      for(int k=0; k<nIndividualsX; k++) {
        GenProgComposite *childX = genProgCompositePopulation[i]->getReplicate();
        childX->mutate();
        genProgCompositePopulationNextGeneration.push_back(childX);
      }
    }

    // FIXME clear genProgCompositePopulation and fill with new individuals, would need access to the Simulation
  }











};
