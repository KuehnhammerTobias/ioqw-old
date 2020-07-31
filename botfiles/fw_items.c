/*
=======================================================================================================================================
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.

This file is part of Spearmint Source Code.

Spearmint Source Code is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

Spearmint Source Code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Spearmint Source Code.
If not, see <http://www.gnu.org/licenses/>.

In addition, Spearmint Source Code is also subject to certain additional terms. You should have received a copy of these additional
terms immediately following the terms and conditions of the GNU General Public License. If not, please request a copy in writing from
id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o
ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.
=======================================================================================================================================
*/

// balance range
#define BR_ARMOR 30
#define BR_HEALTH 30
#define BR_WEAPON 30
#define BR_AMMO 30
#define BR_POWERUP 30
// balance macros
#define MZ(value) (value) < 0 ? 0 : (value)
#define ARMOR_SCALE(v) balance($evalfloat(MZ(FS_ARMOR * v)), $evalfloat(MZ(FS_ARMOR * v - BR_ARMOR)), $evalfloat(MZ(FS_ARMOR * v + BR_ARMOR)))
#define HEALTH_SCALE(v) balance($evalfloat(MZ(FS_HEALTH * v)), $evalfloat(MZ(FS_HEALTH * v - BR_HEALTH)), $evalfloat(MZ(FS_HEALTH * v + BR_HEALTH)))
#define WEAPON_SCALE(v) balance($evalfloat(MZ(v)), $evalfloat(MZ(v - BR_WEAPON)), $evalfloat(MZ(v + BR_WEAPON)))
#define AMMO_SCALE(v) balance($evalfloat(MZ(v)), $evalfloat(MZ(v - BR_AMMO)), $evalfloat(MZ(v + BR_AMMO)))
#define POWERUP_SCALE(v) balance($evalfloat(MZ(v)), $evalfloat(MZ(v - BR_POWERUP)), $evalfloat(MZ(v + BR_POWERUP)))

//=============================================
// HEALTH
//=============================================
weight "item_health_small"
{
	switch (BOT_IS_IN_HURRY)
	{
		case 1:
		{
			switch (INVENTORY_HEALTH)
			{
				case 10: return HEALTH_SCALE(80);
				case 20: return HEALTH_SCALE(80);
				case 30: return HEALTH_SCALE(80);
				case 40: return HEALTH_SCALE(80);
				case 50: return HEALTH_SCALE(70);
				case 60: return HEALTH_SCALE(70);
				case 70: return HEALTH_SCALE(60);
				case 80: return HEALTH_SCALE(60);
				case 90: return HEALTH_SCALE(50);
				case 100: return HEALTH_SCALE(0);
				default: return 0;
			}
		}
		default:
		{
			switch (INVENTORY_HEALTH)
			{
				case 10: return HEALTH_SCALE(40);
				case 20: return HEALTH_SCALE(40);
				case 30: return HEALTH_SCALE(40);
				case 40: return HEALTH_SCALE(20);
				case 50: return HEALTH_SCALE(20);
				case 60: return HEALTH_SCALE(0);
				default: return 0;
			}
		}
	}
}

weight "item_health"
{
	switch (INVENTORY_HEALTH)
	{
		case 10: return HEALTH_SCALE(90);
		case 20: return HEALTH_SCALE(90);
		case 30: return HEALTH_SCALE(90);
		case 40: return HEALTH_SCALE(90);
		case 50: return HEALTH_SCALE(80);
		case 60: return HEALTH_SCALE(80);
		case 70: return HEALTH_SCALE(60);
		case 80: return HEALTH_SCALE(60);
		case 90: return HEALTH_SCALE(50);
		case 100: return HEALTH_SCALE(0);
		default: return 0;
	}
}

weight "item_health_large"
{
	switch (INVENTORY_HEALTH)
	{
		case 10: return HEALTH_SCALE(100);
		case 20: return HEALTH_SCALE(100);
		case 30: return HEALTH_SCALE(100);
		case 40: return HEALTH_SCALE(100);
		case 50: return HEALTH_SCALE(90);
		case 60: return HEALTH_SCALE(90);
		case 70: return HEALTH_SCALE(80);
		case 80: return HEALTH_SCALE(80);
		case 90: return HEALTH_SCALE(50);
		case 100: return HEALTH_SCALE(0);
		default: return 0;
	}
}

weight "item_health_mega"
{
	switch (INVENTORY_HEALTH)
	{
		case 10: return HEALTH_SCALE(110);
		case 20: return HEALTH_SCALE(110);
		case 30: return HEALTH_SCALE(110);
		case 40: return HEALTH_SCALE(110);
		case 50: return HEALTH_SCALE(100);
		case 60: return HEALTH_SCALE(100);
		case 70: return HEALTH_SCALE(90);
		case 80: return HEALTH_SCALE(90);
		case 90: return HEALTH_SCALE(80);
		case 100: return HEALTH_SCALE(0);
		default: return 0;
	}
}

//=============================================
// ARMOR
//=============================================
weight "item_armor_shard"
{
	switch (BOT_IS_IN_HURRY)
	{
		case 1:
		{
			switch (INVENTORY_ARMOR)
			{
				case 10: return ARMOR_SCALE(20);
				case 20: return ARMOR_SCALE(20);
				case 30: return ARMOR_SCALE(20);
				case 40: return ARMOR_SCALE(20);
				case 50: return ARMOR_SCALE(20);
				case 60: return ARMOR_SCALE(15);
				case 70: return ARMOR_SCALE(15);
				case 80: return ARMOR_SCALE(15);
				case 90: return ARMOR_SCALE(15);
				case 100: return ARMOR_SCALE(15);
				case 110: return ARMOR_SCALE(10);
				case 120: return ARMOR_SCALE(10);
				case 130: return ARMOR_SCALE(10);
				case 140: return ARMOR_SCALE(10);
				case 150: return ARMOR_SCALE(10);
				case 160: return ARMOR_SCALE(10);
				case 170: return ARMOR_SCALE(10);
				case 180: return ARMOR_SCALE(10);
				case 190: return ARMOR_SCALE(10);
				case 200: return ARMOR_SCALE(10);
				default: return ARMOR_SCALE(1);
			}
		}
		default: return 0;
	}
}

weight "item_armor_combat"
{
	switch (INVENTORY_ARMOR)
	{
		case 10: return ARMOR_SCALE(60);
		case 20: return ARMOR_SCALE(60);
		case 30: return ARMOR_SCALE(60);
		case 40: return ARMOR_SCALE(60);
		case 50: return ARMOR_SCALE(60);
		case 60: return ARMOR_SCALE(50);
		case 70: return ARMOR_SCALE(50);
		case 80: return ARMOR_SCALE(50);
		case 90: return ARMOR_SCALE(50);
		case 100: return ARMOR_SCALE(50);
		case 110: return ARMOR_SCALE(40);
		case 120: return ARMOR_SCALE(40);
		case 130: return ARMOR_SCALE(40);
		case 140: return ARMOR_SCALE(40);
		case 150: return ARMOR_SCALE(40);
		case 160: return ARMOR_SCALE(40);
		case 170: return ARMOR_SCALE(40);
		case 180: return ARMOR_SCALE(40);
		case 190: return ARMOR_SCALE(40);
		case 200: return ARMOR_SCALE(40);
		default: return ARMOR_SCALE(1);
	}
}

weight "item_armor_body"
{
	switch (INVENTORY_ARMOR)
	{
		case 10: return ARMOR_SCALE(90);
		case 20: return ARMOR_SCALE(90);
		case 30: return ARMOR_SCALE(90);
		case 40: return ARMOR_SCALE(90);
		case 50: return ARMOR_SCALE(90);
		case 60: return ARMOR_SCALE(50);
		case 70: return ARMOR_SCALE(50);
		case 80: return ARMOR_SCALE(50);
		case 90: return ARMOR_SCALE(50);
		case 100: return ARMOR_SCALE(50);
		case 110: return ARMOR_SCALE(20);
		case 120: return ARMOR_SCALE(20);
		case 130: return ARMOR_SCALE(20);
		case 140: return ARMOR_SCALE(20);
		case 150: return ARMOR_SCALE(20);
		case 160: return ARMOR_SCALE(20);
		case 170: return ARMOR_SCALE(20);
		case 180: return ARMOR_SCALE(20);
		case 190: return ARMOR_SCALE(20);
		case 200: return ARMOR_SCALE(20);
		default: return ARMOR_SCALE(1);
	}
}

weight "item_armor_full"
{
	switch (INVENTORY_ARMOR)
	{
		case 10: return ARMOR_SCALE(100);
		case 20: return ARMOR_SCALE(100);
		case 30: return ARMOR_SCALE(100);
		case 40: return ARMOR_SCALE(100);
		case 50: return ARMOR_SCALE(100);
		case 60: return ARMOR_SCALE(70);
		case 70: return ARMOR_SCALE(70);
		case 80: return ARMOR_SCALE(70);
		case 90: return ARMOR_SCALE(70);
		case 100: return ARMOR_SCALE(70);
		case 110: return ARMOR_SCALE(30);
		case 120: return ARMOR_SCALE(30);
		case 130: return ARMOR_SCALE(30);
		case 140: return ARMOR_SCALE(30);
		case 150: return ARMOR_SCALE(30);
		case 160: return ARMOR_SCALE(30);
		case 170: return ARMOR_SCALE(30);
		case 180: return ARMOR_SCALE(30);
		case 190: return ARMOR_SCALE(30);
		case 200: return ARMOR_SCALE(30);
		default: return ARMOR_SCALE(1);
	}
}

//=============================================
// WEAPONS
//=============================================
weight "weapon_machinegun"
{
	switch (INVENTORY_MACHINEGUN)
	{
		case 1:
		{
			switch (INVENTORY_BULLETS)
			{
				case 40: return WEAPON_SCALE(W_MACHINEGUN - 10);
				default: return WEAPON_SCALE(W_MACHINEGUN);
			}
		}
		default:
		{
			switch (BOT_IS_IN_HURRY)
			{
				case 1:
				{
					switch (INVENTORY_BULLETS)
					{
						case 50: return WEAPON_SCALE(GWW_MACHINEGUN);
						case 100: return WEAPON_SCALE(GWW_MACHINEGUN - 10);
						default: return balance(5, 3, 7);
					}
				}
				default:
				{
					switch (INVENTORY_BULLETS)
					{
						case 20: return WEAPON_SCALE(GWW_MACHINEGUN);
						default: return 0;
					}
				}
			}
		}
	}
}

weight "weapon_chaingun"
{
	switch (INVENTORY_CHAINGUN)
	{
		case 1: return WEAPON_SCALE(W_CHAINGUN);
		default:
		{
			switch (BOT_IS_IN_HURRY)
			{
				case 1:
				{
					switch (INVENTORY_BELT)
					{
						case 50: return WEAPON_SCALE(GWW_CHAINGUN);
						case 100: return WEAPON_SCALE(GWW_CHAINGUN - 10);
						default: return balance(5, 3, 7);
					}
				}
				default:
				{
					switch (INVENTORY_BELT)
					{
						case 50: return WEAPON_SCALE(GWW_CHAINGUN);
						default: return 0;
					}
				}
			}
		}
	}
}

weight "weapon_shotgun"
{
	switch (INVENTORY_SHOTGUN)
	{
		case 1:
		{
			switch (INVENTORY_SHELLS)
			{
				case 10: return WEAPON_SCALE(W_SHOTGUN - 10);
				default: return WEAPON_SCALE(W_SHOTGUN);
			}
		}
		default:
		{
			switch (BOT_IS_IN_HURRY)
			{
				case 1:
				{
					switch (INVENTORY_SHELLS)
					{
						case 10: return WEAPON_SCALE(GWW_SHOTGUN);
						case 20: return WEAPON_SCALE(GWW_SHOTGUN - 10);
						default: return balance(5, 3, 7);
					}
				}
				default:
				{
					switch (INVENTORY_SHELLS)
					{
						case 5: return WEAPON_SCALE(GWW_SHOTGUN);
						default: return 0;
					}
				}
			}
		}
	}
}

weight "weapon_nailgun"
{
	switch (INVENTORY_NAILGUN)
	{
		case 1: return WEAPON_SCALE(W_NAILGUN);
		default:
		{
			switch (BOT_IS_IN_HURRY)
			{
				case 1:
				{
					switch (INVENTORY_NAILS)
					{
						case 20: return WEAPON_SCALE(GWW_NAILGUN);
						case 40: return WEAPON_SCALE(GWW_NAILGUN - 10);
						default: return balance(5, 3, 7);
					}
				}
				default:
				{
					switch (INVENTORY_NAILS)
					{
						case 20: return WEAPON_SCALE(GWW_NAILGUN);
						default: return 0;
					}
				}
			}
		}
	}
}

weight "weapon_proxylauncher"
{
	switch (BOT_IS_IN_HURRY)
	{
		case 1:
		{
			switch (INVENTORY_PROXLAUNCHER)
			{
				case 1: return WEAPON_SCALE(W_PROXLAUNCHER);
				default:
				{
					switch (INVENTORY_MINES)
					{
						case 5: return WEAPON_SCALE(GWW_PROXLAUNCHER);
						case 10: return WEAPON_SCALE(GWW_PROXLAUNCHER - 10);
						default: return balance(5, 3, 7);
					}
				}
			}
		}
		default: return 0;
	}
}

weight "weapon_grenadelauncher"
{
	switch (BOT_IS_IN_HURRY)
	{
		case 1:
		{
			switch (INVENTORY_GRENADELAUNCHER)
			{
				case 1: return WEAPON_SCALE(W_GRENADELAUNCHER);
				default:
				{
					switch (INVENTORY_GRENADES)
					{
						case 20: return WEAPON_SCALE(GWW_GRENADELAUNCHER);
						case 40: return WEAPON_SCALE(GWW_GRENADELAUNCHER - 10);
						default: return balance(5, 3, 7);
					}
				}
			}
		}
		default: return 0;
	}
}

weight "weapon_napalmlauncher"
{
	switch (INVENTORY_NAPALMLAUNCHER)
	{
		case 1: return WEAPON_SCALE(W_NAPALMLAUNCHER);
		default:
		{
			switch (BOT_IS_IN_HURRY)
			{
				case 1:
				{
					switch (INVENTORY_CANISTERS)
					{
						case 20: return WEAPON_SCALE(GWW_NAPALMLAUNCHER);
						case 40: return WEAPON_SCALE(GWW_NAPALMLAUNCHER - 10);
						default: return balance(5, 3, 7);
					}
				}
				default:
				{
					switch (INVENTORY_CANISTERS)
					{
						case 10: return WEAPON_SCALE(GWW_NAPALMLAUNCHER);
						default: return 0;
					}
				}
			}
		}
	}
}

weight "weapon_rocketlauncher"
{
	switch (INVENTORY_ROCKETLAUNCHER)
	{
		case 1: return WEAPON_SCALE(W_ROCKETLAUNCHER);
		default:
		{
			switch (BOT_IS_IN_HURRY)
			{
				case 1:
				{
					switch (INVENTORY_ROCKETS)
					{
						case 20: return WEAPON_SCALE(GWW_ROCKETLAUNCHER);
						case 40: return WEAPON_SCALE(GWW_ROCKETLAUNCHER - 10);
						default: return balance(5, 3, 7);
					}
				}
				default:
				{
					switch (INVENTORY_ROCKETS)
					{
						case 10: return WEAPON_SCALE(GWW_ROCKETLAUNCHER);
						default: return 0;
					}
				}
			}
		}
	}
}

weight "weapon_beamgun"
{
	switch (INVENTORY_BEAMGUN)
	{
		case 1: return WEAPON_SCALE(W_BEAMGUN);
		default:
		{
			switch (BOT_IS_IN_HURRY)
			{
				case 1:
				{
					switch (INVENTORY_BEAMGUN_AMMO)
					{
						case 50: return WEAPON_SCALE(GWW_BEAMGUN);
						case 80: return WEAPON_SCALE(GWW_BEAMGUN - 10);
						default: return balance(5, 3, 7);
					}
				}
				default:
				{
					switch (INVENTORY_BEAMGUN_AMMO)
					{
						case 50: return WEAPON_SCALE(GWW_BEAMGUN);
						default: return 0;
					}
				}
			}
		}
	}
}

weight "weapon_railgun"
{
	switch (INVENTORY_RAILGUN)
	{
		case 1: return WEAPON_SCALE(W_RAILGUN);
		default:
		{
			switch (BOT_IS_IN_HURRY)
			{
				case 1:
				{
					switch (INVENTORY_SLUGS)
					{
						case 10: return WEAPON_SCALE(GWW_RAILGUN);
						case 20: return WEAPON_SCALE(GWW_RAILGUN - 10);
						default: return balance(5, 3, 7);
					}
				}
				default:
				{
					switch (INVENTORY_SLUGS)
					{
						case 5: return WEAPON_SCALE(GWW_RAILGUN);
						default: return 0;
					}
				}
			}
		}
	}
}

weight "weapon_plasmagun"
{
	switch (INVENTORY_PLASMAGUN)
	{
		case 1: return WEAPON_SCALE(W_PLASMAGUN);
		default:
		{
			switch (BOT_IS_IN_HURRY)
			{
				case 1:
				{
					switch (INVENTORY_CELLS)
					{
						case 20: return WEAPON_SCALE(GWW_PLASMAGUN);
						case 50: return WEAPON_SCALE(GWW_PLASMAGUN - 10);
						default: return balance(5, 3, 7);
					}
				}
				default:
				{
					switch (INVENTORY_CELLS)
					{
						case 20: return WEAPON_SCALE(GWW_PLASMAGUN);
						default: return 0;
					}
				}
			}
		}
	}
}

weight "weapon_bfg"
{
	switch (INVENTORY_BFG10K)
	{
		case 1: return WEAPON_SCALE(W_BFG10K);
		default:
		{
			switch (INVENTORY_BFG_AMMO)
			{
				case 50: return WEAPON_SCALE(GWW_BFG10K);
				case 200: return WEAPON_SCALE(GWW_BFG10K - 10);
				default: return balance(5, 3, 7);
			}
		}
	}
}

//=============================================
// AMMO
//=============================================
weight "ammo_bullets"
{
	switch (BOT_IS_IN_HURRY)
	{
		case 1:
		{
			switch (INVENTORY_BULLETS)
			{
				case 20: return AMMO_SCALE(20);
				case 40: return AMMO_SCALE(20);
				case 60: return AMMO_SCALE(20);
				case 80: return AMMO_SCALE(20);
				case 100: return AMMO_SCALE(20);
				case 120: return AMMO_SCALE(20);
				case 140: return AMMO_SCALE(20);
				case 160: return AMMO_SCALE(20);
				case 180: return AMMO_SCALE(20);
				case 200: return AMMO_SCALE(0);
				default: return 0;
			}
		}
		default: return 0;
	}
}

weight "ammo_belt"
{
	switch (BOT_IS_IN_HURRY)
	{
		case 1:
		{
			switch (INVENTORY_BELT)
			{
				case 20: return AMMO_SCALE(20);
				case 40: return AMMO_SCALE(20);
				case 60: return AMMO_SCALE(20);
				case 80: return AMMO_SCALE(20);
				case 100: return AMMO_SCALE(20);
				case 120: return AMMO_SCALE(20);
				case 140: return AMMO_SCALE(20);
				case 160: return AMMO_SCALE(20);
				case 180: return AMMO_SCALE(20);
				case 200: return AMMO_SCALE(0);
				default: return 0;
			}
		}
		default: return 0;
	}
}

weight "ammo_shells"
{
	switch (BOT_IS_IN_HURRY)
	{
		case 1:
		{
			switch (INVENTORY_SHELLS)
			{
				case 20: return AMMO_SCALE(20);
				case 40: return AMMO_SCALE(20);
				case 60: return AMMO_SCALE(20);
				case 80: return AMMO_SCALE(20);
				case 100: return AMMO_SCALE(20);
				case 120: return AMMO_SCALE(20);
				case 140: return AMMO_SCALE(20);
				case 160: return AMMO_SCALE(20);
				case 180: return AMMO_SCALE(20);
				case 200: return AMMO_SCALE(0);
				default: return 0;
			}
		}
		default: return 0;
	}
}

weight "ammo_nails"
{
	switch (BOT_IS_IN_HURRY)
	{
		case 1:
		{
			switch (INVENTORY_NAILS)
			{
				case 20: return AMMO_SCALE(20);
				case 40: return AMMO_SCALE(20);
				case 60: return AMMO_SCALE(20);
				case 80: return AMMO_SCALE(20);
				case 100: return AMMO_SCALE(20);
				case 120: return AMMO_SCALE(20);
				case 140: return AMMO_SCALE(20);
				case 160: return AMMO_SCALE(20);
				case 180: return AMMO_SCALE(20);
				case 200: return AMMO_SCALE(0);
				default: return 0;
			}
		}
		default: return 0;
	}
}

weight "ammo_mines"
{
	switch (BOT_IS_IN_HURRY)
	{
		case 1:
		{
			switch (INVENTORY_MINES)
			{
				case 20: return AMMO_SCALE(20);
				case 40: return AMMO_SCALE(20);
				case 60: return AMMO_SCALE(20);
				case 80: return AMMO_SCALE(20);
				case 100: return AMMO_SCALE(20);
				case 120: return AMMO_SCALE(20);
				case 140: return AMMO_SCALE(20);
				case 160: return AMMO_SCALE(20);
				case 180: return AMMO_SCALE(20);
				case 200: return AMMO_SCALE(0);
				default: return 0;
			}
		}
		default: return 0;
	}
}

weight "ammo_grenades"
{
	switch (BOT_IS_IN_HURRY)
	{
		case 1:
		{
			switch (INVENTORY_GRENADES)
			{
				case 20: return AMMO_SCALE(20);
				case 40: return AMMO_SCALE(20);
				case 60: return AMMO_SCALE(20);
				case 80: return AMMO_SCALE(20);
				case 100: return AMMO_SCALE(20);
				case 120: return AMMO_SCALE(20);
				case 140: return AMMO_SCALE(20);
				case 160: return AMMO_SCALE(20);
				case 180: return AMMO_SCALE(20);
				case 200: return AMMO_SCALE(0);
				default: return 0;
			}
		}
		default: return 0;
	}
}

weight "ammo_canisters"
{
	switch (BOT_IS_IN_HURRY)
	{
		case 1:
		{
			switch (INVENTORY_CANISTERS)
			{
				case 20: return AMMO_SCALE(20);
				case 40: return AMMO_SCALE(20);
				case 60: return AMMO_SCALE(20);
				case 80: return AMMO_SCALE(20);
				case 100: return AMMO_SCALE(20);
				case 120: return AMMO_SCALE(20);
				case 140: return AMMO_SCALE(20);
				case 160: return AMMO_SCALE(20);
				case 180: return AMMO_SCALE(20);
				case 200: return AMMO_SCALE(0);
				default: return 0;
			}
		}
		default: return 0;
	}
}

weight "ammo_rockets"
{
	switch (BOT_IS_IN_HURRY)
	{
		case 1:
		{
			switch (INVENTORY_ROCKETS)
			{
				case 20: return AMMO_SCALE(20);
				case 40: return AMMO_SCALE(20);
				case 60: return AMMO_SCALE(20);
				case 80: return AMMO_SCALE(20);
				case 100: return AMMO_SCALE(20);
				case 120: return AMMO_SCALE(20);
				case 140: return AMMO_SCALE(20);
				case 160: return AMMO_SCALE(20);
				case 180: return AMMO_SCALE(20);
				case 200: return AMMO_SCALE(0);
				default: return 0;
			}
		}
		default: return 0;
	}
}

weight "ammo_cartridge"
{
	switch (BOT_IS_IN_HURRY)
	{
		case 1:
		{
			switch (INVENTORY_BEAMGUN_AMMO)
			{
				case 20: return AMMO_SCALE(20);
				case 40: return AMMO_SCALE(20);
				case 60: return AMMO_SCALE(20);
				case 80: return AMMO_SCALE(20);
				case 100: return AMMO_SCALE(20);
				case 120: return AMMO_SCALE(20);
				case 140: return AMMO_SCALE(20);
				case 160: return AMMO_SCALE(20);
				case 180: return AMMO_SCALE(20);
				case 200: return AMMO_SCALE(0);
				default: return 0;
			}
		}
		default: return 0;
	}
}

weight "ammo_slugs"
{
	switch (BOT_IS_IN_HURRY)
	{
		case 1:
		{
			switch (INVENTORY_SLUGS)
			{
				case 20: return AMMO_SCALE(20);
				case 40: return AMMO_SCALE(20);
				case 60: return AMMO_SCALE(20);
				case 80: return AMMO_SCALE(20);
				case 100: return AMMO_SCALE(20);
				case 120: return AMMO_SCALE(20);
				case 140: return AMMO_SCALE(20);
				case 160: return AMMO_SCALE(20);
				case 180: return AMMO_SCALE(20);
				case 200: return AMMO_SCALE(0);
				default: return 0;
			}
		}
		default: return 0;
	}
}

weight "ammo_cells"
{
	switch (BOT_IS_IN_HURRY)
	{
		case 1:
		{
			switch (INVENTORY_CELLS)
			{
				case 20: return AMMO_SCALE(20);
				case 40: return AMMO_SCALE(20);
				case 60: return AMMO_SCALE(20);
				case 80: return AMMO_SCALE(20);
				case 100: return AMMO_SCALE(20);
				case 120: return AMMO_SCALE(20);
				case 140: return AMMO_SCALE(20);
				case 160: return AMMO_SCALE(20);
				case 180: return AMMO_SCALE(20);
				case 200: return AMMO_SCALE(0);
				default: return 0;
			}
		}
		default: return 0;
	}
}

weight "ammo_bfg"
{
	switch (INVENTORY_BFG_AMMO)
	{
		case 20: return AMMO_SCALE(20);
		case 40: return AMMO_SCALE(20);
		case 60: return AMMO_SCALE(20);
		case 80: return AMMO_SCALE(20);
		case 100: return AMMO_SCALE(20);
		case 120: return AMMO_SCALE(20);
		case 140: return AMMO_SCALE(20);
		case 160: return AMMO_SCALE(20);
		case 180: return AMMO_SCALE(20);
		case 200: return AMMO_SCALE(0);
		default: return 0;
	}
}

//=============================================
// POWERUPS
//=============================================

weight "holdable_medkit"
{
	switch (INVENTORY_MEDKIT)
	{
		case 1:
		{
			switch (INVENTORY_KAMIKAZE)
			{
				case 1: return POWERUP_SCALE(W_MEDKIT);
				default: return 0; //already has a holdable
			}
		}
		default: return 0; //already has a holdable
	}
}

weight "holdable_kamikaze"
{
	switch (BOT_IS_IN_HURRY)
	{
		case 1:
		{
			switch (INVENTORY_MEDKIT)
			{
				case 1:
				{
					switch (INVENTORY_KAMIKAZE)
					{
						case 1: return POWERUP_SCALE(W_KAMIKAZE);
						default: return 0; //already has a holdable
					}
				}
				default: return 0; //already has a holdable
			}
		}
		default: return 0; // bot has an emergency goal
	}
}

weight "item_quad"
{
	return POWERUP_SCALE(W_QUAD);
}

weight "item_invisibility"
{
	return POWERUP_SCALE(W_INVISIBILITY);
}

weight "item_regen"
{
	return POWERUP_SCALE(W_REGEN);
}

weight "item_ammoregen"
{
	switch (INVENTORY_AMMOREGEN)
	{
		case 1:
		{
			switch (INVENTORY_GUARD)
			{
				case 1:
				{
					switch (INVENTORY_DOUBLER)
					{
						case 1:
						{
							switch (INVENTORY_SCOUT)
							{
								case 1: return POWERUP_SCALE(W_AMMOREGEN);
								default: return 0;
							}
						}
						default: return 0;
					}
				}
				default: return 0;
			}
		}
		default: return 0;
	}
}

weight "item_guard"
{
	switch (INVENTORY_AMMOREGEN)
	{
		case 1:
		{
			switch (INVENTORY_GUARD)
			{
				case 1:
				{
					switch (INVENTORY_DOUBLER)
					{
						case 1:
						{
							switch (INVENTORY_SCOUT)
							{
								case 1: return POWERUP_SCALE(W_GUARD);
								default: return 0;
							}
						}
						default: return 0;
					}
				}
				default: return 0;
			}
		}
		default: return 0;
	}
}

weight "item_doubler"
{
	switch (INVENTORY_AMMOREGEN)
	{
		case 1:
		{
			switch (INVENTORY_GUARD)
			{
				case 1:
				{
					switch (INVENTORY_DOUBLER)
					{
						case 1:
						{
							switch (INVENTORY_SCOUT)
							{
								case 1: return POWERUP_SCALE(W_DOUBLER);
								default: return 0;
							}
						}
						default: return 0;
					}
				}
				default: return 0;
			}
		}
		default: return 0;
	}
}

weight "item_scout"
{
	switch (INVENTORY_AMMOREGEN)
	{
		case 1:
		{
			switch (INVENTORY_GUARD)
			{
				case 1:
				{
					switch (INVENTORY_DOUBLER)
					{
						case 1:
						{
							switch (INVENTORY_SCOUT)
							{
								case 1: return POWERUP_SCALE(W_SCOUT);
								default: return 0;
							}
						}
						default: return 0;
					}
				}
				default: return 0;
			}
		}
		default: return 0;
	}
}

//=======================================================================================================================
// This is only used to pickup dropped CTF flags now. The logic in here makes no sense since the bot has specific CTF AI.
//=======================================================================================================================

weight "team_CTF_redflag"
{
	return 400;
}

weight "team_CTF_blueflag"
{
	return 400;
}

weight "team_CTF_neutralflag"
{
	return 400;
}

weight "item_redcube"
{
	return 400;
}

weight "item_bluecube"
{
	return 400;
}

//=============================================
// bot roam goal
//=============================================

weight "item_botroam"
{
	return 1;
}
