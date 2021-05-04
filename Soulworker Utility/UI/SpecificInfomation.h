#pragma once

#define DEBUG_MONSTER_COMBO 0

class SpecificInformation : public MemoryPool<SpecificInformation, 10> {
private:
	UINT32 _playerID;
	UINT32 _monsterID;
	
	FLOAT _globalFontScale;
	FLOAT _columnFontScale;
	FLOAT _tableFontScale;

	VOID UpdateSkillInfo();
	VOID UpdateMonsterCombo();
	VOID UpdateSkillTable();
	VOID DrawBar(FLOAT window_Width, FLOAT percent, ImU32 color);

	VOID SetupFontScale();
	VOID Clear();

	SpecificInformation() : _playerID(0), _monsterID(0), _globalFontScale(0), _columnFontScale(0), _tableFontScale(0) {}
public:
	SpecificInformation(UINT32 playerID);
	~SpecificInformation();

	VOID Update(BOOL* open);
};