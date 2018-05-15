//        Farhad   Mohammad Kazemi

#include <ctime>
#include <cmath>
#include <fstream>
#include <iostream>

#include "Config.h"
#include "GenerationalTournamentPopulation.h"

#include "InstructionArgumentConstant.h"
#include "InstructionArgumentFeature.h"
#include "InstructionArgumentRegister.h"

#include "InstructionOperationPlus.h"
#include "InstructionOperationMinus.h"
#include "InstructionOperationMult.h"
#include "InstructionOperationDiv.h"
#include "InstructionOperationIflt.h"

#include "SymRegFitnessCase.h"

#include "SymRegProgram.h"

using namespace VUWLGP; // else the lines of code get stupidly and clumsily long

// Initialise the static members which specify the proportion selected through elitism and
// the size of the selection tournaments for other evolutionary operators:
double GenerationalTournamentPopulation<SymRegProgram<double>, double>::proportionElitism
= 0.1;
unsigned int GenerationalTournamentPopulation<SymRegProgram<double>, double>::tournamentSize
= 2;

int main(int argc, char** argv) {
	// Building the relevant config object - specify it all, even though not all of it is needed:
	Config<double>* c;
	if (argc == 1) {
		c = new Config<double>();
	}
	else {
		c = new Config<double>(argc, argv, true);
	}

	c->numRegisters = 6;
	c->numFeatures = 1;
	c->epsilon = 0.1;

	c->argumentGenerators->AddElement(InstructionArgumentConstant<double>::Generate);
	c->argumentGenerators->AddElement(InstructionArgumentFeature<double>::Generate);
	c->argumentGenerators->AddElement(InstructionArgumentRegister<double>::Generate);

	c->instructionOperations->AddElement(InstructionOperationPlus<double>::Generate);
	c->instructionOperations->AddElement(InstructionOperationMinus<double>::Generate);
	c->instructionOperations->AddElement(InstructionOperationMult<double>::Generate);
	c->instructionOperations->AddElement(InstructionOperationDiv<double>::Generate);
	c->instructionOperations->AddElement(InstructionOperationIflt<double>::Generate);

	c->initialMinLength = 6;
	c->initialMaxLength = 10;
	c->maxLength = 200;
	c->populationSize = 1000;

	c->maxGenerations = 1000;

	c->popLogInterval = 1;
	c->popLogFilePath = "population";
	c->statsLogFilePath = "log";

	c->runLogFilePath = "results.csv";

	// Initialise the RNG:
	if (c->seedSpecified) {
		Rand::Init(c->randSeed);
	}
	else {
		Rand::Init();
	}

	// Building the population object, randomising the fitness of it all:
	IPopulation<SymRegProgram<double>, double>* pop =
		new GenerationalTournamentPopulation<SymRegProgram<double>, double>(c);

	// Build the fitness environment we will train and another we will test on:
	FitnessEnvironment<double> train(c);
	for (double i = -4; i <= 4; i += 0.02) {
		train.AddCase(new SymRegFitnessCase<double>(i, (1 - ((i*i) / 4) - ((i*i) / 4))*exp(-((i*i) / 8) - ((i*i) / 8))));
	}

	FitnessEnvironment<double> test(c);
	for (double i = -8; i <= 8; i += 0.04) {
				test.AddCase(new SymRegFitnessCase<double>(i, (1 - ((i*i) / 4) - ((i*i) / 4))*exp(-((i*i) / 8) - ((i*i) / 8))));
	}

	std::cout << "Loaded " << train.NumberOfCases() << " training cases and "
		<< test.NumberOfCases() << " test cases. Beginning evolution." << std::endl;


	// Carry out the evolution:
	//clock();
	unsigned int generationsUsedOrNoSolution = pop->Evolve(train);
	int time = clock();

	if (generationsUsedOrNoSolution <= c->maxGenerations) {
		std::cout << "Solution found.\nSolution is:" << std::endl;
	}
	else {
		std::cout << "No solution found.\nBest program:" << std::endl;
	}

	IProgram<double>* best = pop->GetFittestProgram();
	std::cout << pop->GetFittestProgram()->ToString(true, true) << std::endl;

	// Now calculate the statistics that aren't already calculated:
	std::cout << "Writing run statistics to: " << c->runLogFilePath << std::endl;

	// Time is already set, generations is set as it is because if no solution is found it comes
	// back set to maxGen + 1 (to distinguish the return value from "solution found in last gen").
	// Test fitness still needs to be calculated for the best program.
	unsigned int generations = std::min(generationsUsedOrNoSolution, c->maxGenerations);

	double bestTrainingFitness = best->Fitness();

	best->UpdateFitness(test);
	double bestTestFitness = best->Fitness();

	std::ofstream fout(c->runLogFilePath.c_str(), std::ios::out | std::ios::app);
	fout << generations << "," << time << "," << bestTrainingFitness << "," << bestTestFitness
		<< std::endl;
	std::cout << generations << "," << time << "," << bestTrainingFitness << ","
		<< bestTestFitness << std::endl;
	std::cout << "\n\t(NB: Clock ticks per second: " << CLK_TCK << ")" << std::endl;
	fout.close();

	delete pop;
	delete c;
	return 0;
}
