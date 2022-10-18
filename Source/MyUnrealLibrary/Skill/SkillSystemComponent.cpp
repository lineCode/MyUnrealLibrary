#include "SkillSystemComponent.h"
#include "Skill.h"
#include "../Ability/CharacterAttributeSet.h"
#include "../Character/PlayableCharacter.h"

TArray<USkill*> USkillSystemComponent::GetUnlockedSkills()
{
	TArray<USkill*> UnlockedSkills;

	for (int i = 0; i < Skills.Num(); ++i)
	{
		if (Skills[i]->IsSkillUnlocked())
		{
			UnlockedSkills.Add(Skills[i]);
		}
	}

	return UnlockedSkills;
}

USkill* USkillSystemComponent::GetSkillByClass(TSubclassOf<USkill> SkillClass, bool bExactMatching)
{
	if (bExactMatching)
	{
		for (int i = 0; i < Skills.Num(); ++i)
		{
			if (Skills[i]->GetClass() == SkillClass)
				return Skills[i];
		}
	}
	else
	{
		for (int i = 0; i < Skills.Num(); ++i)
		{
			if (Skills[i]-IsA(SkillClass))
				return Skills[i];
		}
	}
	
	return nullptr;
}

USkill* USkillSystemComponent::GetSkillByID(FName SkillID)
{
	for (int i = 0; i < Skills.Num(); ++i)
	{
		if (Skills[i]->SkillID == SkillID)
		{
			return Skills[i];
		}
	}

	return nullptr;
}

APlayableCharacter* USkillSystemComponent::GetOwningCharacter()
{
	return Cast<APlayableCharacter>(GetOwner());
}

bool USkillSystemComponent::HasPrerequisiteSkills(TArray<FName> PrereqSkills)
{
	TArray<USkill*> UnlockedSkills = GetUnlockedSkills();

	for (int i = 0; i < PrereqSkills.Num(); ++i)
	{
		bool bFound = false;

		for (int j = 0; j < UnlockedSkills.Num(); ++i)
		{
			if (UnlockedSkills[i]->SkillID == PrereqSkills[i])
			{
				bFound = true;
				break;
			}
		}

		if (!bFound)
		{
			return false;
		}
	}

	return true;
}

void USkillSystemComponent::SetSkillEnabled(FName SkillID, bool bEnabled)
{
	USkill* Skill = GetSkillByID(SkillID);

	if (!Skill)
		return;

	// To do: perhaps refactor the bool getter
	bool bSuccess = Skill->SetSkillEnabled(bEnabled);

	if (!bSuccess)
		return;

	APlayableCharacter* Character = GetOwningCharacter();

	if (!Character)
		return;

	if (bEnabled)
	{
		Character->LearnAbility(Skill->AbilityClass, Character->GetCharacterAttributeSet()->GetLevel());
	}
	else
	{
		Character->ForgetAbility(Skill->AbilityClass);
	}

	OnSkillEnabledStateChanged.Broadcast(Skill, bEnabled);
}

void USkillSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// To do: if can read save data, then initialize from save data

	for (int i = 0; i < SkillSet.Num(); ++i)
	{
		USkill* NewSkill = NewObject<USkill>(this, SkillSet[i]);
		Skills.Add(NewSkill);
	}
}

void USkillSystemComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	// To do: handle the saving of the component data
}
