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

#include "syn.h"

CONTEXT_NEARBYITEM
{
	// health
	[("Mega Health", 0), ("MH", 0)]
	// armor
	[("Armor", 0), ("Green Armor", 0), ("Armour", 0), ("Green Armour", 0), ("GA", 0)]
	[("Heavy Armor", 0), ("Yellow Armor", 0), ("Yellow Armour", 0), ("YA", 0)]
	[("Full Armor", 0), ("Red Armor", 0), ("Heavy Armour", 0), ("Red Armour", 0), ("RA", 0)]
	// weapons
	[("Machine Gun", 1), ("MG", 0)]
	[("Chain Gun", 1), ("CG", 0)]
	[("Shotgun", 1), ("SG", 0.5)]
	[("Nail Gun", 1), ("NG", 0)]
	[("Proximity Launcher", 1), ("Proxlauncher", 0.7), ("Proximitylauncher", 0.5)]
	[("Grenade Launcher", 1), ("GL", 0.7)]
	[("Napalm Launcher", 1), ("NL", 0.7)]
	[("Rocket Launcher", 1), ("RL", 0.7)]
	[("Beam Gun", 1), ("BG", 0.7)]
	[("Railgun", 1), ("Rail", 0.5), ("RG", 0)]
	[("Plasma Gun", 1), ("Plasma", 0.7), ("PG", 0)]
	[("BFG 10K", 1), ("BFG", 1)]
	// powerups
	[("Medkit", 1), ("Syringe", 0)]
	[("Kamikaze", 1), ("Kamikazi", 0)]
	[("Quad Damage", 1), ("Quad", 0.7)]
	[("Invisibility", 1), ("Invis", 0)]
	[("Regeneration", 1), ("Regen", 0.6)]
	[("Ammo Regen", 1), ("Ammoregen", 0.5)]
	[("Guard", 1), ("Guard", 0.5)]
	[("Doubler", 1), ("Doubler", 0.5)]
	[("Scout", 1), ("Scout", 0.5)]
	[("Neutral Flag", 0), ("White Flag", 0)]
	[("Neutral Obelisk", 1), ("Skull Fountain", 0), ("Center Obelisk", 0)]
}

CONTEXT_CTFREDTEAM
{
	[("Red Flag", 0), ("our base", 0), ("our flag", 0), ("red base", 0), ("base", 0)]
	[("Blue Flag", 0), ("enemy base", 0), ("blue base", 0), ("enemy flag", 0)]
}

CONTEXT_CTFBLUETEAM
{
	[("Red Flag", 0), ("enemy flag", 0), ("red base", 0), ("enemy base", 0)]
	[("Blue Flag", 0), ("our base", 0), ("our flag", 0), ("blue base", 0), ("base", 0)]
}

CONTEXT_OBELISKREDTEAM
{
	[("Red Obelisk", 0), ("our base", 0), ("red base", 0), ("base", 1), ("our obelisk", 0)]
	[("Blue Obelisk", 0), ("enemy base", 0), ("blue base", 0), ("enemy obelisk", 0)]
}

CONTEXT_OBELISKBLUETEAM
{
	[("Red Obelisk", 0), ("red base", 0), ("enemy base", 1), ("enemy obelisk", 0)]
	[("Blue Obelisk", 0), ("our base", 0), ("blue base", 0), ("base", 1), ("our obelisk", 0)]
}

CONTEXT_HARVESTERREDTEAM
{
	[("Red Obelisk", 0), ("our base", 0), ("red base", 0), ("base", 1), ("our obelisk", 0)]
	[("Blue Obelisk", 0), ("enemy base", 0), ("blue base", 0), ("enemy obelisk", 0)]
}

CONTEXT_HARVESTERBLUETEAM
{
	[("Red Obelisk", 0), ("red base", 0), ("enemy base", 1), ("enemy obelisk", 0)]
	[("Blue Obelisk", 0), ("our base", 0), ("blue base", 0), ("base", 1), ("our obelisk", 0)]
}

CONTEXT_NORMAL
{
	// contractions
	[("do not", 1), ("don't", 1), ("dont", 0)]
	[("does not", 1), ("doesn't", 1), ("doesnt", 0)]
	[("did not", 1), ("didn't", 1), ("didnt", 0)]
	[("have not", 1), ("haven't", 1), ("havent", 0)]
	[("has not", 1), ("hasn't", 1), ("hasnt", 0)]
	[("had not", 1), ("hadn't", 1), ("hadnt", 0)]
	[("will not", 1), ("won't", 1), ("wont", 0)]
	[("would not", 1), ("wouldn't", 1), ("wouldnt", 0)]
	[("should not", 1), ("shoudn't", 1), ("shoudnt", 0)]
	[("can not", 1), ("can't", 1), ("cant", 0), ("cannot", 0)]
	[("could not", 1), ("couldn't", 1), ("couldnt", 0)]
	[("is not", 1), ("isn't", 1), ("isnt", 0), ("ain't", 0.2), ("aint", 0)]
	[("must not", 1), ("mustn't", 1), ("mustnt", 0)]
	// are
	[("I am", 1), ("I'm", 1)]
	[("you are", 1), ("you're", 1)]
	[("he is", 1), ("he's", 1), ("hes", 0)]
	[("she is", 1), ("she's", 1), ("shes", 0)]
	[("it is", 1), ("it's", 1), ("its", 0)]
	[("they are", 1), ("they're", 1), ("theyre", 0)]
	[("we are", 1), ("we're", 0.7)] // , ("were", 1) conflicts
	[("what is", 1), ("what's", 1), ("whats", 0)]
	[("that is", 1), ("that's", 1), ("thats", 0)]
	[("how is", 1), ("how's", 1), ("hows", 0)]
	[("why is", 1), ("why's", 1), ("whys", 0)]
	[("who is", 1), ("who's", 1), ("whos", 0)]
	[("here is", 1), ("here's", 1), ("heres", 0)]
	[("there is", 1), ("there's", 1), ("theres", 0)]
	[("where is", 1), ("where's", 1), ("wheres", 0)]
	// will
	[("I will", 1), ("I'll", 1)]
	[("you will", 1), ("you'll", 1)]
	[("he will", 1), ("he'll", 1)]
	[("she will", 1), ("she'll", 1)]
	[("it will", 1), ("it'll", 1)]
	[("they will", 1), ("they'll", 1), ("theyll", 0)]
	[("we will", 1), ("we'll", 1)]
	[("how will", 1), ("how'll", 1)]
	[("that will", 1), ("that'll", 1), ("thatll", 0)]
	// would
	[("I would", 1), ("I'd", 1)]
	[("you would", 1), ("you'd", 1)]
	[("he would", 1), ("he'd", 1)]
	[("she would", 1), ("she'd", 1)]
	[("it would", 1), ("it'd", 1)]
	[("they would", 1), ("they'd", 1)]
	[("we would", 1), ("we'd", 1)]
	// have
	[("I have", 1), ("I've", 0)]
	[("you have", 1), ("you've", 0)]
// 	[("he has", 1), ("he's", 1)] // conflicting with "he is"
// 	[("she has", 1), ("she's", 1)] // conflicting with "she is"
// 	[("it has", 1), ("it's", 1)] // conflicting with "it is"
	[("they have", 1), ("they've", 1)]
	[("we have", 1), ("we've", 1)]
// 	[("I had", 1), ("I'd", 1), ("Id")] // conflicting with "I would"
// 	[("you had", 1), ("you'd", 1), ("youd", 1)] // conflicting with "you would"
// 	[("he had", 1), ("he'd", 1), ("hed", 1)] // conflicting with "he would"
// 	[("she had", 1), ("she'd", 1)] // conflicting with "she would"
// 	[("it had", 1), ("it'd", 1)] // conflicting with "it would"
// 	[("we had", 1), ("we'd", 1)] // conflicting with "we would"
	// ing chops
	[("being", 1), ("bein", 0)]
	[("going", 1), ("goin", 0)]
	[("having", 1), ("havin", 0)]
	[("doing", 1), ("doin", 0)]
	[("hanging", 1), ("hangin", 0)]
	[("living", 1), ("livin", 0)]
	// slang
	[("cool", 1), ("kool", 1), ("kwl", 1), ("kewl", 0.5)]
	[("you", 1), ("U", 0.01)]
	[("your", 1), ("yer", 0)]
	[("out of", 1), ("outta", 0.07)]
	[("are", 1), ("R", 0.02)]
	[("why", 1), ("Y", 0.02)]
	[("people", 1), ("ppl", 0.05), ("folks", 0.5)]
	[("thanx", 0.5), ("thanks", 1), ("tx", 0), ("tnx", 0), ("thx", 0.5), ("thnx", 0.6)]
	[("this", 1), ("diz", 0.05), ("dis", 0.1)]
	[("men", 1), ("boyz", 0.3), ("guys", 0.2), ("d00dz", 0)]
	[("women", 1), ("girlz", 0.1), ("Grrrlz", 0.2)]
	[("picture", 1), ("pic", 1)]
	[("pictures", 1), ("pics", 1), ("pix", 0)]
	[("everyone", 1), ("everybody", 1)]
	[("see you", 1), ("cya", 0), ("cu", 0), ("c u", 0)]
	[("father", 1), ("dad", 1), ("pa", 0), ("daddy", 0.6), ("poppa", 1)]
	[("mother", 1), ("mom", 1), ("mommy", 1), ("mama", 1), ("momma", 1), ("muthah", 0)]
	[("brother", 1), ("bro", 0.2), ("bruthah", 0)]
	[("sister", 1), ("sis", 0.4), ("sistah", 0)]
	[("butt", 1), ("ass", 1), ("heinie", 0.3), ("bum", 0.4), ("rear", 0.4), ("rear end", 0.5), ("bottom", 1)]

	[("going to", 1), ("gonna", 0.02)]
	[("got to", 1), ("gotta", 0.05)]
	[("want to", 1), ("wanna", 0.05)]

	[("waypoint", 1), ("way-point", 1), ("wp", 1)]
	[("checkpoint", 1), ("check-point", 1), ("cp", 1)]

	[("defense", 1), ("defence", 0)]
	[("offense", 1), ("offence", 0)]
	// numbers
	[("0", 1), ("zero", 1)]
	[("1", 1), ("one", 1)]
	[("2", 1), ("two", 1)]
	[("3", 1), ("three", 1)]
	[("4", 1), ("four", 1)]
	[("5", 1), ("five", 1)]
	[("6", 1), ("six", 1)]
	[("7", 1), ("seven", 1)]
	[("8", 1), ("eight", 1)]
	[("9", 1), ("nine", 1)]
	[("10", 1), ("ten", 1)]
	[("11", 1), ("eleven", 1)]
	[("12", 1), ("twelve", 1)]
	[("13", 1), ("thirteen", 1)]
	[("14", 1), ("fourteen", 1)]
	[("15", 1), ("fifteen", 1)]
	[("16", 1), ("sixteen", 1)]
	[("17", 1), ("seventeen", 1)]
	[("18", 1), ("eighteen", 1)]
	[("19", 1), ("nineteen", 1)]
	[("20", 1), ("twenty", 1)]
	[("30", 1), ("thirty", 1)]
	[("40", 1), ("fourty", 1)]
	[("50", 1), ("fifty", 1)]
	[("60", 1), ("sixty", 1)]
	[("70", 1), ("seventy", 1)]
	[("80", 1), ("eighty", 1)]
	[("90", 1), ("ninety", 1)]
	[("100", 1), ("a hundred", 1), ("one hundred", 1)]
	[("1000", 1), ("a thousand", 1), ("one thousand", 1)]
	[("100000", 1), ("a hundred thousand", 1), ("one hundred thousand", 1)]
	[("1000000", 1), ("a million", 1), ("one million", 1)]
}

CONTEXT_REPLY
{
	[("that I", 0), ("that you", 0)]
	[("that you", 0), ("that I", 0)]
	[("you", 0), ("I", 0)]
	[("my", 0), ("your", 0)]
	[("your", 0), ("my", 0)]
	[("you", 0), ("me", 0)]
	[("me", 0), ("you", 0)]
	[("mine", 0), ("yours", 0)]
	[("yours", 0), ("mine", 0)]
	[("am", 0), ("are", 0)]
	[("are", 0), ("am", 0)]
	[("yourself", 0), ("myself", 0)]
	[("myself", 0), ("yourself", 0)]
}

CONTEXT_NAMES
{
	[("Fritzkrieg", 1), ("Fritz", 0.9), ("Blitzkrieg", 0.5), ("Kraut", 0.5), ("Ubersoldat", 0.4), ("Stinkstiefel", 0.3)]
	[("Grunt", 1), ("Marine", 0.9), ("Leatherneck", 0.5), ("Bruce", 0.4)]
	[("Hossman", 1), ("Big Fella", 0.8), ("Big Guy", 0.4), ("Mikey", 0.2)]
	[("Klansman", 1), ("White Man", 0.7)]
	[("Klesk", 1), ("Bug", 0.3)]
	[("Lucy", 1), ("Pattie-Fattie", 0.7), ("Lard Butt", 0.2)]
	[("Major", 1), ("Cyclops", 0.2), ("Boss Lady", 0.6)]
	[("Pi", 1), ("3.14", 0.4), ("Pee", 0.3)]
	[("Ranger", 1), ("Old Timer", 0.3), ("Q Dude", 0.7), ("Quaker", 0.4)]
	[("Sarge", 1), ("Old Man", 0.5), ("Miles", 0.5), ("Hero", 0.6)]
	[("Sorlag", 1), ("Lizard Breath", 0.4), ("Sorg Scum", 0.4), ("S-s-s-snake", 0.5)]
	[("Visor", 1), ("Vizor", 0.9), ("Voice-box", 0.3)]
	[("Xaerena", 1), ("Mrs. X", 0.3), ("X Lady", 0.1), ("Lady X", 0)]
	[("Xaero", 1), ("Honored Master", 0.3), ("Number 1", 0.1), ("Mr. X", 0.4), ("Numero Uno", 0.3)]
}
