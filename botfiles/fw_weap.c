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

weight "Gauntlet"
{
	switch (INVENTORY_GAUNTLET)
	{
		case 1: return 0;
		default: return W_GAUNTLET;
	}
}

weight "Machine Gun"
{
	switch (INVENTORY_MACHINEGUN)
	{
		case 1: return 0;
		default:
		{
			switch (INVENTORY_BULLETS)
			{
				case 1: return 0;
				default:
				{
					switch (ENTITY_IS_AN_OBELISK)
					{
						case 1: return W_MACHINEGUN;
						default:
						{
							switch (ENEMY_HORIZONTAL_DIST)
							{
								case 500: return 40;
								default: return W_MACHINEGUN;
							}
						}
					}
				}
			}
		}
	}
}

weight "Chain Gun"
{
	switch (INVENTORY_CHAINGUN)
	{
		case 1: return 0;
		default:
		{
			switch (INVENTORY_BELT)
			{
				case 1: return 0;
				default:
				{
					switch (ENTITY_IS_AN_OBELISK)
					{
						case 1:
						{
							switch (ENEMY_HORIZONTAL_DIST)
							{
								case 2000: return W_CHAINGUN;
								default: return $evalint(W_MACHINEGUN + 1); // Tobias TODO: try to fix this! (should be W_PHOSPHORGUN - 1)
							}
						}
						default:
						{
							switch (ENEMY_HORIZONTAL_DIST)
							{
								case 500: return 520;
								default: return W_CHAINGUN;
							}
						}
					}
				}
			}
		}
	}
}

weight "Shotgun"
{
	switch (INVENTORY_SHOTGUN)
	{
		case 1: return 0;
		default:
		{
			switch (INVENTORY_SHELLS)
			{
				case 1: return 0;
				default:
				{
					switch (ENTITY_IS_AN_OBELISK)
					{
						case 1:
						{
							switch (ENEMY_HORIZONTAL_DIST)
							{
								case 750: return W_SHOTGUN;
								default: return 100;
							}
						}
						default:
						{
							switch (ENEMY_HORIZONTAL_DIST)
							{
								case 500: return 120;
								default: return 100;
							}
						}
					}
				}
			}
		}
	}
}

weight "Nail Gun"
{
	switch (INVENTORY_NAILGUN)
	{
		case 1: return 0;
		default:
		{
			switch (INVENTORY_NAILS)
			{
				case 1: return 0;
				default:
				{
					switch (ENTITY_IS_AN_OBELISK)
					{
						case 1:
						{
							switch (ENEMY_HORIZONTAL_DIST)
							{
								case 2000: return W_NAILGUN;
								default: return 100;
							}
						}
						default:
						{
							switch (ENEMY_HORIZONTAL_DIST)
							{
								case 500: return 540;
								default: return W_NAILGUN;
							}
						}
					}
				}
			}
		}
	}
}

weight "Proximity Launcher"
{
	switch (INVENTORY_PROXLAUNCHER)
	{
		case 1: return 0;
		default:
		{
			switch (INVENTORY_MINES)
			{
				case 1: return 0;
				default:
				{
					switch (ENEMY_HORIZONTAL_DIST)
					{
						case 2000: return W_PROXLAUNCHER;
						default: return $evalint(W_PROXLAUNCHER * 0.1);
					}
				}
			}
		}
	}
}

weight "Grenade Launcher"
{
	switch (INVENTORY_GRENADELAUNCHER)
	{
		case 1: return 0;
		default:
		{
			switch (INVENTORY_GRENADES)
			{
				case 1: return 0;
				default:
				{
					switch (ENEMY_HORIZONTAL_DIST)
					{
						case 2000: return W_GRENADELAUNCHER;
						default: return $evalint(W_GRENADELAUNCHER * 0.1);
					}
				}
			}
		}
	}
}

weight "Napalm Launcher"
{
	switch (INVENTORY_NAPALMLAUNCHER)
	{
		case 1: return 0;
		default:
		{
			switch (INVENTORY_CANISTERS)
			{
				case 1: return 0;
				default:
				{
					switch (ENTITY_IS_AN_OBELISK)
					{
						case 1:
						{
							switch (ENEMY_HORIZONTAL_DIST)
							{
								case 2000: return W_NAPALMLAUNCHER;
								default: return 100;
							}
						}
						default:
						{
							switch (ENEMY_HORIZONTAL_DIST)
							{
								case 500: return 540;
								default: return W_NAPALMLAUNCHER;
							}
						}
					}
				}
			}
		}
	}
}

weight "Rocket Launcher"
{
	switch (INVENTORY_ROCKETLAUNCHER)
	{
		case 1: return 0;
		default:
		{
			switch (INVENTORY_ROCKETS)
			{
				case 1: return 0;
				default:
				{
					switch (ENTITY_IS_AN_OBELISK)
					{
						case 1:
						{
							switch (ENEMY_HORIZONTAL_DIST)
							{
								case 2000: return W_ROCKETLAUNCHER;
								default: return 101;
							}
						}
						default:
						{
							switch (ENEMY_HORIZONTAL_DIST)
							{
								case 500: return 460;
								default: return W_ROCKETLAUNCHER;
							}
						}
					}
				}
			}
		}
	}
}

weight "Beam Gun"
{
	switch (INVENTORY_BEAMGUN)
	{
		case 1: return 0;
		default:
		{
			switch (INVENTORY_BEAMGUN_AMMO)
			{
				case 1: return 0;
				default:
				{
					switch (ENTITY_IS_AN_OBELISK)
					{
						case 1:
						{
							switch (ENEMY_HORIZONTAL_DIST)
							{
								case 750: return W_BEAMGUN;
								default: return $evalint(W_BEAMGUN * 0.1);
							}
						}
						default:
						{
							switch (ENEMY_HORIZONTAL_DIST)
							{
								case 500: return 480;
								default: return $evalint(W_BEAMGUN * 0.1);
							}
						}
					}
				}
			}
		}
	}
}

weight "Railgun"
{
	switch (INVENTORY_RAILGUN)
	{
		case 1: return 0;
		default:
		{
			switch (INVENTORY_SLUGS)
			{
				case 1: return 0;
				default:
				{
					switch (ENTITY_IS_AN_OBELISK)
					{
						case 1:
						{
							switch (ENEMY_HORIZONTAL_DIST)
							{
								case 4000: return W_RAILGUN;
								default: return 580;
							}
						}
						default:
						{
							switch (ENEMY_HORIZONTAL_DIST)
							{
								case 500: return 100;
								case 4000: return 120;
								default: return 580;
							}
						}
					}
				}
			}
		}
	}
}

weight "Plasma Gun"
{
	switch (INVENTORY_PLASMAGUN)
	{
		case 1: return 0;
		default:
		{
			switch (INVENTORY_CELLS)
			{
				case 1: return 0;
				default:
				{
					switch (ENTITY_IS_AN_OBELISK)
					{
						case 1: return W_PLASMAGUN;
						default:
						{
							switch (ENEMY_HORIZONTAL_DIST)
							{
								case 500: return 500;
								default: return W_PLASMAGUN;
							}
						}
					}
				}
			}
		}
	}
}

weight "BFG 10K"
{
	switch (INVENTORY_BFG10K)
	{
		case 1: return 0;
		default:
		{
			switch (INVENTORY_BFG_AMMO)
			{
				case 1: return 0;
				default:
				{
					switch (ENTITY_IS_AN_OBELISK)
					{
						case 1: return W_BFG10K;
						default:
						{
							switch (ENEMY_HORIZONTAL_DIST)
							{
								case 500: return 560;
								default: return W_BFG10K;
							}
						}
					}
				}
			}
		}
	}
}
