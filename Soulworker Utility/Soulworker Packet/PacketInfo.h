#pragma once


enum StatType {
	CurrentHP = 0x01,
	SG = 0x02,
	Stamina = 0x03,
	STR = 0x04,
	DEX = 0x05,
	INTELLIGENCE = 0x06,
	BALANCE = 0x07,
	CON = 0x08,
	LUK = 0x09,
	MaxHP = 0x0a,
	HPRegen = 0x0b,
	MaxSG = 0x0c,
	SGRegen = 0x0d,
	MaxStamina = 0x0e,
	StaminaRegen = 0x0f,
	SV = 0x10,
	MaxSV = 0x11,
	MoveSpeed = 0x12,
	AttackSpeed = 0x13,
	MinAttack = 0x14,
	MaxAttack = 0x15,
	MinMagicAttack = 0x16,
	MaxMagicAttack = 0x17,
	Defense = 0x18,
	MagicDefense = 0x19,
	Accuracy = 0x1a,
	MagicAccuracy = 0x1b,
	PartialDamage = 0x1c,
	Crit = 0x1d,
	MagicCrit = 0x1e,
	CritResist = 0x1f,
	MagicCritResist = 0x20,
	/* Use Crit, MagicCrit Instead */
	// CritChance = 0x21,
	// MagicCritChance = 0x22,
	CritDamage = 0x23,
	MagicCritDamage = 0x24,
	// Damage reduction chance(%) = 0x25,
	DamageReduction = 0x26,
	MagicDamageReduction = 0x27,
	// Resistance to permanent status ailments(%) = 0x28,
	// Resistance to manipulation status(%) = 0x29,
	// Resistance to special status abnormalities(%) = 0x2a,
	Evade = 0x2b,
	MagicEvade = 0x2c,
	// Attack Success Rate (Disabled) = 0x2d,
	// Magic attack success rate (disabled) = 0x2e,
	ArmorBreak = 0x2f,
	MagicArmorBreak = 0x30,
	// 0x31 ~ 0x3c : status effect resist
	// 0x3f ~ 0x44 : soulstone damage
	SoulStone_Hatred = 0x42
	// 0x45 ~ 0x4A : soulstone resist

};
#pragma pack(push, 1)
typedef struct _SWPACKETDAMAGE_DAMAGETYPE {
	unsigned int MISS : 1; // Gamnabit 0x01
	unsigned int unknown2 : 1;
	unsigned int CRIT : 1; // critical 0x04
	unsigned int unknown4 : 1;
	unsigned int soulstoneType : 4;

	_SWPACKETDAMAGE_DAMAGETYPE(BYTE rawData) {
		MISS = rawData & 0x01 ? 1 : 0;
		unknown2 = rawData & 0x02 ? 1 : 0;
		CRIT = rawData & 0x04 ? 1 : 0;
		unknown4 = rawData & 0x08 ? 1 : 0;
		soulstoneType = rawData >> 4;
	};
}SWPACKETDAMAGE_DAMAGETYPE;
#pragma pack(pop)