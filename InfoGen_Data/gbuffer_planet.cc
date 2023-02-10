#include "gbuffer_planet.h"

using namespace std;
using namespace cse;

OrbitParam ParentOrbitBuffer;
float64 ParentLumBuffer;
vector<float64> ParentLumBufferVec;
vector<OrbitParam> ParentOrbitBufferVec;

string GenPlanetType(shared_ptr<Object> Obj)
{
	if (Obj->Class == "Ferria") { return "Iron planet"; }
	if (Obj->Class == "Terra") { return "Rocky planet"; }
	if (Obj->Class == "Aquaria")
	{
		if (!Obj->NoOcean && Obj->Ocean.Height > 15000)
		{
			return "Ocean planet";
		}
		return "Ice planet";
	}
	if (Obj->Class == "Carbonia") { return "Carbon planet"; }
	if (Obj->Class == "Neptune" || Obj->Class == "IceGiant")
	{
		if (Obj->Teff > 700 && Obj->Teff < 1800) { return "Hoptune"; }
		if (Obj->Teff >= 1800) { return "Ultra-hot Neptune"; }
		if (Obj->Radius() > 5. * RadEarth) { return "Super-Neptune"; }
		if (Obj->Radius() < 3. * RadEarth) { return "Gas dwarf"; } // Mini-Neptune
		return "Ice giant";
	}
	if (Obj->Class == "Jupiter" || Obj->Class == "GasGiant")
	{
		// Gas Giants using Sudarsky's gas giant classification
		// https://en.wikipedia.org/wiki/Sudarsky%27s_gas_giant_classification
		if (Obj->Teff < 150) { return "Class I(Ammonia clouds) Gas giant"; }
		if (Obj->Teff >= 150 && Obj->Teff < 250) { return "Class II(Water clouds) Gas giant"; }
		if (Obj->Teff >= 250 && Obj->Teff < 800) { return "Class III(Cloudless) Gas giant"; }
		if (Obj->Teff >= 800 && Obj->Teff < 1400) { return "Class IV(Alkali metals) Gas giant"; }
		if (Obj->Teff >= 1400) { return "Class V(Silicate clouds) Gas giant"; }
		return "Gas Giant";
	}
	if (Obj->Class == "Chthonia") { return "Helium Planet"; }
	if (Obj->Class == "Asteroid") { return "Minor Planet"; }
	return "Planet";
}

PlanetParams gbuffer_planet(shared_ptr<Object> Companion)
{
	PlanetParams Par;

	Par.MeanRadius = Companion->Radius();
	Par.EqRadius = cse::max({ Companion->Dimensions.x, Companion->Dimensions.y, Companion->Dimensions.z }) / 2.;
	Par.PolarRadius = Companion->Dimensions.y / 2.;
	Par.Flattening = (Par.EqRadius - Par.PolarRadius) / Par.EqRadius;
	Par.Volume = (4. / 3.) * CSE_PI * (Companion->Dimensions.x / 2.) * (Companion->Dimensions.y / 2.) * (Companion->Dimensions.z / 2.);
	Par.Mass = Companion->Mass;
	Par.MeanDensity = Par.Mass / Par.Volume;
	Par.SurfGrav = (GravConstant * Par.Mass) / cse::pow(Par.EqRadius, 2);
	Par.EscapeVel = cse::sqrt(2. * Par.SurfGrav * Par.EqRadius);
	Par.SynodicPeriod = (Companion->Orbit.Period * Companion->Rotation.RotationPeriod) / (Companion->Orbit.Period - Companion->Rotation.RotationPeriod);
	Par.RotationPeriod = Companion->Rotation.RotationPeriod;
	Par.AxialTilt = Companion->Rotation.Obliquity;
	if ((Par.AxialTilt > 90 && Par.AxialTilt < 270) || (Par.AxialTilt < -90 && Par.AxialTilt > -270))
	{
		Par.SynodicPeriod = (Companion->Orbit.Period * Companion->Rotation.RotationPeriod) / (Companion->Orbit.Period + Companion->Rotation.RotationPeriod);
		Par.SynodicPeriod *= -1;
		Par.RotationPeriod *= -1;
	}
	Par.AlbedoBond = Companion->AlbedoBond;
	Par.AlbedoGeom = Companion->AlbedoGeom;

	// Stefan-Boltzmann law can also use for planets.
	// https://astronomy.stackexchange.com/questions/10113/how-to-calculate-the-expected-surface-temperature-of-a-planet
	bool BinaryPlanet = false;
	if (ParentBodyStack.top().Pointer->Type == "Barycenter" && ParentBodyStack.top().Pointer->Class == NO_DATA_STRING)
	{
		ParentBodyStack.pop();
		BinaryPlanet = true;
	}

	if (ParentBodyStack.top().Pointer->Type == "Star" || (ParentBodyStack.top().Pointer->Type == "Barycenter" && ParentBodyStack.top().Pointer->Class == "Sun"))
	{
		float64 Aphelion = 2. * Companion->Orbit.SemiMajorAxis() - Companion->Orbit.PericenterDist;
		float64 SemiMajorAxis = Companion->Orbit.SemiMajorAxis();
		float64 PeriHelion = Companion->Orbit.PericenterDist;
		float64 Luminosity = ParentBodyStack.top().Pointer->LumBol;
		Companion->Teff = yroot(4, (Luminosity * (1. - Par.AlbedoBond)) / (16. * CSE_PI * cse::pow(SemiMajorAxis, 2) * StBConstant));

		Par.MinTemperature = yroot(4, (Luminosity * (1. - Par.AlbedoBond)) / (16. * CSE_PI * cse::pow(Aphelion, 2) * StBConstant));
		Par.MeanTemperature = yroot(4, (Luminosity * (1. - Par.AlbedoBond)) / (16. * CSE_PI * cse::pow(SemiMajorAxis, 2) * StBConstant));
		Par.MaxTemperature = yroot(4, (Luminosity * (1. - Par.AlbedoBond)) / (16. * CSE_PI * cse::pow(PeriHelion, 2) * StBConstant));

		// Backtrack the parent body stack, and add the effect from companion stars
		auto PBS = ParentBodyStack; // copy a backup for the stack
		auto Parent = PBS.top();
		PBS.pop(); // record the planet's parent star and remove it from stack

		bool NoSMA = false;
		if (isinf(Parent.Pointer->Orbit.SemiMajorAxis()))
		{
			NoSMA = true;
		}

		ParentLumBufferVec.clear();
		ParentOrbitBufferVec.clear();

		while (!PBS.empty())
		{
			SystemStruct::Elem Comp;
			for (size_t i = 0; i < PBS.top().SubSystem->Catalog.size(); i++)
			{
				if 
				(
					(Parent.Pointer->Orbit.RefPlane == NO_DATA_STRING &&
					PBS.top().SubSystem->Catalog[i].Pointer->Orbit.Binary) ||
					(PBS.top().SubSystem->Catalog[i].Pointer->Orbit.RefPlane == NO_DATA_STRING &&
					Parent.Pointer->Orbit.Binary)
				)
				{
					Comp = PBS.top().SubSystem->Catalog[i];
					break;
				}
			}

			float64 Lum = 0;
			OrbitParam POrbit = 
			{
				.PericenterDist = 1,
				.Eccentricity = 0
			};
			if (Comp.Pointer != nullptr)
			{
				if (NoSMA)
				{
					POrbit = Comp.Pointer->Orbit;
					Lum = Comp.Pointer->LumBol;
				}
				else
				{
					POrbit = Parent.Pointer->Orbit;
					Lum = Comp.Pointer->LumBol;
				}

				float64 AphelionP = 2. * POrbit.SemiMajorAxis() - POrbit.PericenterDist;
				float64 SemiMajorAxisP = POrbit.SemiMajorAxis();
				float64 PeriHelionP = POrbit.PericenterDist;
				float64 AddTemp0 = yroot(4, (Lum * (1. - Par.AlbedoBond)) / (16. * CSE_PI * cse::pow(AphelionP, 2) * StBConstant));
				float64 AddTemp1 = yroot(4, (Lum * (1. - Par.AlbedoBond)) / (16. * CSE_PI * cse::pow(SemiMajorAxisP, 2) * StBConstant));
				float64 AddTemp2 = yroot(4, (Lum * (1. - Par.AlbedoBond)) / (16. * CSE_PI * cse::pow(PeriHelionP, 2) * StBConstant));
				Par.MinTemperature += AddTemp0;
				Par.MeanTemperature += AddTemp1;
				Par.MaxTemperature += AddTemp2;

				if (CurrentSubSystem != nullptr || BinaryPlanet)
				{
					ParentLumBufferVec.push_back(Lum);
					ParentOrbitBufferVec.push_back(POrbit);
				}
			}

			Parent = PBS.top();
			PBS.pop();
		}

		if (CurrentSubSystem != nullptr || BinaryPlanet)
		{
			ParentOrbitBuffer = Companion->Orbit;
			ParentLumBuffer = Luminosity;
		} // Export orbit data and parent luminosity, used to calculate companion's temperature.
	}
	else
	{
		float64 Aphelion = 2. * ParentOrbitBuffer.SemiMajorAxis() - ParentOrbitBuffer.PericenterDist;
		float64 SemiMajorAxis = ParentOrbitBuffer.SemiMajorAxis();
		float64 PeriHelion = ParentOrbitBuffer.PericenterDist;
		float64 Luminosity = ParentLumBuffer;
		Companion->Teff = yroot(4, (Luminosity * (1. - Par.AlbedoBond)) / (16. * CSE_PI * cse::pow(SemiMajorAxis, 2) * StBConstant));

		Par.MinTemperature = yroot(4, (Luminosity * (1. - Par.AlbedoBond)) / (16. * CSE_PI * cse::pow(Aphelion, 2) * StBConstant));
		Par.MeanTemperature = yroot(4, (Luminosity * (1. - Par.AlbedoBond)) / (16. * CSE_PI * cse::pow(SemiMajorAxis, 2) * StBConstant));
		Par.MaxTemperature = yroot(4, (Luminosity * (1. - Par.AlbedoBond)) / (16. * CSE_PI * cse::pow(PeriHelion, 2) * StBConstant));

		for (size_t i = 0; i < ParentLumBufferVec.size(); i++)
		{
			float64 AphelionP = 2. * ParentOrbitBufferVec[i].SemiMajorAxis() - ParentOrbitBufferVec[i].PericenterDist;
			float64 SemiMajorAxisP = ParentOrbitBufferVec[i].SemiMajorAxis();
			float64 PeriHelionP = ParentOrbitBufferVec[i].PericenterDist;
			float64 AddTemp0 = yroot(4, (ParentLumBufferVec[i] * (1. - Par.AlbedoBond)) / (16. * CSE_PI * cse::pow(AphelionP, 2) * StBConstant));
			float64 AddTemp1 = yroot(4, (ParentLumBufferVec[i] * (1. - Par.AlbedoBond)) / (16. * CSE_PI * cse::pow(SemiMajorAxisP, 2) * StBConstant));
			float64 AddTemp2 = yroot(4, (ParentLumBufferVec[i] * (1. - Par.AlbedoBond)) / (16. * CSE_PI * cse::pow(PeriHelionP, 2) * StBConstant));
			Par.MinTemperature += AddTemp0;
			Par.MeanTemperature += AddTemp1;
			Par.MaxTemperature += AddTemp2;
		}
	}

	if (BinaryPlanet)
	{
		ParentBodyStack.push(SystemStruct::Elem{ .Pointer = Companion });
	}

	return Par;
}