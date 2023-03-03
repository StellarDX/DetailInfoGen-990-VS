#pragma once

#ifndef __IG_COMMON__
#define __IG_COMMON__

#include <CSE/Object.h>

static const cse::Object __StelCSE_Earth // An Earth scale model.
{
	.Type          = "Planet",
	.Name          = {"Earth"},
	.ParentBody    = "Sun",

	.Mass          = 1.0 * MassEarth,			// Earth's masses
	.Dimensions    = 2. * cse::vec3(6378137, 6356752.3141, 6378137),     // meters, IUGG value

	.AlbedoBond    = 0.306,
	.AlbedoGeom    = 0.367,

	.Teff          = 14.76 + 273.15, // Mean
	.Age           = 4.54E9,

	.Brightness    = 1.0,
	.BrightnessReal= 1.0,
	.Color         = cse::vec3(0.850, 0.850, 1.000),

	.Rotation
	{
		.RotationPeriod= SiderealDay,  // secs
		.Obliquity     = 23.4392811,  // degrees
		.EqAscendNode  = 180.0,		// degrees
		.RotationOffset= -23.9,        // degrees
		.Precession    = 25592,       // period of precession in years
	},

	.IAU // SE Data
	{
		.Epoch		  = 2451545,		// J2000
		.PoleRA		  = 0,			// degrees
		.PoleRARate	  = -0.641,		// degrees/century
		.PoleDec	  = 90,			// degrees
		.PoleDecRate  = -0.557,		// degrees/century
		.PrimeMeridian= 190.147,		// degrees
		.RotationRate = 360.9856235	// degrees/day
	},

	.Orbit // Epoch J2000
	{
		.AnalyticModel="EMB-DE",
		.RefPlane     ="Ecliptic",
		.Period       = SiderealYear,          // secs
		.PericenterDist= 147098450000, // meters
		.Eccentricity = 0.0167086342,
		.Inclination  = 0.00005,          // degrees
		.AscendingNode= -11.26064,      // degrees
		.ArgOfPericenter= 114.20783, // degrees
		.MeanAnomaly  = 358.617, // degrees
	},

	.Life
	{{
		.Class = "Organic",
		.Type = "Multicellular",
		.Biome = {"Marine", "Terrestrial"}
	}, {}},

	.Interior
	{
		{"Ices",      2.444e-4},
		{"Silicates", 68      },
		{"Metals",    32      }
	},

	.Surface
	{
		// Surface map author: Robert Skuridin "PBC"
		.DiffMap      = "Earth/Surface-PBC",
		.DiffMapAlpha = "Water",

		// Elevation map author: Robert Skuridin "PBC"
		.BumpMap      = "Earth/Bump-PBC",
		.BumpHeight   =  19.475, // = 10896 + 8579 meters of total elevation span
		.BumpOffset   =  10.896, // = 10896 meters below sea level

		// City lights map author: Sean Young "HarbingerDawn", based on John van Vliet's map
		.GlowMap      = "Earth/Lights-HD",
		.GlowMode     = "Night",
		.GlowColor    = cse::vec3(1.0, 1.0, 1.0),	// map itself is RGB
		.GlowBright   =  1.0,

		.SpecParams   = cse::vec4(0.0, 0.35, 0, 0)
		//.colorSea      =(0.012 0.03 0.06 1.00)
		//.colorShelf    =(0.15 0.48 0.46 1.00)
	},

	.Ocean
	{
		.Height=10.896,

		.Composition // values in percent
		{
			{"H2O",  96.5},
			{"NaCl", 3.5}
		}
	},

	.Clouds
	{
		.Layer
		{{
			// Clouds map author: Vladimir Romanyuk "SpaceEngineer"
			.DiffMap      = "Earth/Clouds-SE",

			.BumpMap      = "Earth/Clouds-SE",
			.Height       =  9.0, //3.5
			.Velocity     =  0.0, // km/h
			.BumpHeight   =  0.5,
			.BumpOffset   =  0.0,

			//DayAmbient      3.0
			.Hapke        =  0.2,
			.ModulateColor= cse::vec4(1.0, 1.0, 1.0, 1.333),
			.ModulateBright= 1.333
		}}
	},

	.Atmosphere
	{
		.Model     ="Earth",
		.Height    = 60,		// km
		.Density   = 1.2929,	// kg/m^3
		.Adiabat   = 0.28,
		.Pressure  = 1.0 * StdAtm,		// atm
		.Greenhouse= 33,		// degrees K
		.Bright    = 10.0,
		.Opacity   = 1.0,
		.SkyLight  = 3.0,
		
		.Composition // values in percent
		{
			{"N2", 78.08  },
			{"O2", 20.95  },
			{"Ar", 0.9340 },
			{"H2O",1      },
			{"CO2",0.0413 },
			{"Ne", 0.00182},
			{"He", 0.00052},
			{"CH4",0.00019},
			{"Kr", 0.00011},
			{"H2", 0.00006}
			//{"Xe", 0.0000087}
		}
	},

	.Aurora
	{
		.Height     = 100,   // km

		.North
		{
			.Lat   = 82,    // degrees
			.Lon   =-113,   // degrees
			.Radius= 2500,  // km
			.Width = 600,   // km
			.Rings = 3,     // number of rings
			.Bright= 0.3
		},

		.South
		{
			.Lat   =-63,    // degrees
			.Lon   = 138,   // degrees
			.Radius= 2000,  // km
			.Width = 600,   // km
			.Rings = 3,     // number of rings
			.Bright= 0.3
		},

		.TopColor   =cse::vec3(0.5, 0.5, 0.5),	// multiplier
        .BottomColor=cse::vec3(0.0, 1.0, 0.0),	// color
	},

	.NoRings = true,
};

#endif