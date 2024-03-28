// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameState.h"


ACustomGameState::ACustomGameState() :
	Score(0),
	StationsSpawnable(2)
{
	StateTracker = StateEnum::MainMenu;
}