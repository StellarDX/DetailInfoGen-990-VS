#include "gbuffer_star.h"
#include <CSE/Planets.h>

using namespace std;
using namespace cse;

namespace Localization
{
	static string Hypergiant  = "Hypergiant";
	static string BSG         = "BSG";
	static string YSG         = "YSG";
	static string RSG         = "RSG";
	static string Supergiant  = "Supergiant";
	static string BlueGiant   = "Blue giant";
	static string YellowGiant = "Yellow giant";
	static string RedGiant    = "Red giant";
	static string GiantStar   = "Giant star";
	static string Subgiant    = "Subgiant";
	static string OTypeStar   = "O-type main-sequence star";
	static string BTypeStar   = "B-type main-sequence star";
	static string ATypeStar   = "A-type main-sequence star";
	static string FTypeStar   = "F-type main-sequence star";
	static string GTypeStar   = "Yellow dwarf";
	static string KTypeStar   = "Orange dwarf";
	static string MTypeStar   = "Red dwarf";
	static string MSStar      = "Main-Sequence";
	static string sdO         = "sdO";
	static string sdB         = "sdB";
	static string Subdwarf    = "Subdwarf";
	static string WolfRayet   = "Wolf-Rayet Star";
	static string LTypeStar   = "L-type dwarf";
	static string TTypeStar   = "Brown dwarf";
	static string YTypeStar   = "Sub-brown dwarf";
	static string BrownDwarf  = "Substellar object";
	static string STypeStar   = "S-type star";
	static string CarbonStar  = "Carbon star";
	static string WhiteDwarf  = "White dwarf";
	static string NeutronStar = "Pulsar";
	static string BlackHoleSW = "Schwarzschild Black hole";
	static string BlackHoleKR = "Kerr Black hole";
	static string BlackHoleRN = "Reissner-Nordstrom Black hole";
	static string BlackHoleKN = "Kerr-Newman Black hole";
	static string BlackHole   = "Black hole";
	static string OtherStar   = "Star";
}

using namespace Localization;

string GenStarType(shared_ptr<Object> Pointer)
{
	SPECSTR Spec = Pointer->SpecClass;
	if (IsHyperGiant(Spec)){return Hypergiant;}
	if (IsSuperGiant(Spec))
	{
		if (IsOType(Spec) || IsBType(Spec) || IsAType(Spec)){return BSG;}
		if (IsFType(Spec) || IsGType(Spec)) { return YSG; }
		if (IsKType(Spec) || IsMType(Spec) || IsLType(Spec)) { return RSG; }
		return Supergiant;
	}
	if (IsGiant(Spec))
	{
		if (IsOType(Spec) || IsBType(Spec) || IsAType(Spec)) { return BlueGiant; }
		if (IsFType(Spec) || IsGType(Spec)) { return YellowGiant; }
		if (IsKType(Spec) || IsMType(Spec)) { return RedGiant; }
		return GiantStar;
	}
	if (IsSubGiant(Spec)) { return Subgiant; }
	if (IsMainSequence(Spec))
	{
		if (IsOType(Spec)) { return OTypeStar; }
		if (IsBType(Spec)) { return BTypeStar; }
		if (IsAType(Spec)) { return ATypeStar; }
		if (IsFType(Spec)) { return FTypeStar; }
		if (IsGType(Spec)) { return GTypeStar; }
		if (IsKType(Spec)) { return KTypeStar; }
		if (IsMType(Spec)) { return MTypeStar; }
		return MSStar;
	}
	if (IsSubDwarf(Spec))
	{
		if (IsOType(Spec)) { return sdO; }
		if (IsBType(Spec)) { return sdB; }
		return Subdwarf;
	}
	if (IsWolfRayet(Spec)) { return WolfRayet; }
	if (IsBrownDwarf(Spec))
	{
		if (IsLType(Spec)) { return LTypeStar; }
		if (IsTType(Spec)) { return TTypeStar; }
		if (IsYType(Spec)) { return YTypeStar; }
		return BrownDwarf;
	}
	if (IsCarbonStar(Spec))
	{
		if (Spec.SClass() >= 28 && Spec.SClass() <= 30) { return STypeStar; }
		return CarbonStar;
	}
	if (IsWhiteDwarf(Spec)) { return WhiteDwarf; }
	if (IsNeutronStar(Spec)) { return NeutronStar; }
	if (IsBlackHole(Spec))
	{
		if (isinf(Pointer->KerrSpin)) { Pointer->KerrSpin = 0; }
		if (isinf(Pointer->KerrCharge)) { Pointer->KerrCharge = 0; }
		if (Pointer->KerrSpin == 0 && Pointer->KerrCharge == 0) { return BlackHoleSW; }
		if (Pointer->KerrSpin != 0 && Pointer->KerrCharge == 0) { return BlackHoleKR; }
		if (Pointer->KerrSpin == 0 && Pointer->KerrCharge != 0) { return BlackHoleRN; }
		if (Pointer->KerrSpin != 0 && Pointer->KerrCharge != 0) { return BlackHoleKN; }
		return BlackHole;
	}
	return OtherStar;
}

BlackHoleParams BlackHolePar(shared_ptr<Object> arg)
{
	return
	{
		.Mass = arg->Mass,
		.SchwarzschildRadius = SchwarzschildRadius(arg->Mass),
		.Spin = arg->KerrSpin,
		.Charge = arg->KerrCharge
	};
}

void GetLcString(string Key, string* Val);

void GetLocaleStar()
{
	GetLcString("Hypergiant",  &Hypergiant);
	GetLcString("BSG",         &BSG);
	GetLcString("YSG",         &YSG);
	GetLcString("RSG",         &RSG);
	GetLcString("Supergiant",  &Supergiant);
	GetLcString("BlueGiant",   &BlueGiant);
	GetLcString("YellowGiant", &YellowGiant);
	GetLcString("RedGiant",    &RedGiant);
	GetLcString("GiantStar",   &GiantStar);
	GetLcString("Subgiant",    &Subgiant);
	GetLcString("OTypeStar",   &OTypeStar);
	GetLcString("BTypeStar",   &BTypeStar);
	GetLcString("ATypeStar",   &ATypeStar);
	GetLcString("FTypeStar",   &FTypeStar);
	GetLcString("GTypeStar",   &GTypeStar);
	GetLcString("KTypeStar",   &KTypeStar);
	GetLcString("MTypeStar",   &MTypeStar);
	GetLcString("MSStar",      &MSStar);
	GetLcString("sdO",         &sdO);
	GetLcString("sdB",         &sdB);
	GetLcString("Subdwarf",    &Subdwarf);
	GetLcString("LTypeStar",   &LTypeStar);
	GetLcString("TTypeStar",   &TTypeStar);
	GetLcString("YTypeStar",   &YTypeStar);
	GetLcString("BrownDwarf",  &BrownDwarf);
	GetLcString("STypeStar",   &STypeStar);
	GetLcString("CarbonStar",  &CarbonStar);
	GetLcString("WhiteDwarf",  &WhiteDwarf);
	GetLcString("NeutronStar", &NeutronStar);
	GetLcString("BlackHoleSW", &BlackHoleSW);
	GetLcString("BlackHoleKR", &BlackHoleKR);
	GetLcString("BlackHoleRN", &BlackHoleRN);
	GetLcString("BlackHoleKN", &BlackHoleKN);
	GetLcString("BlackHole",   &BlackHole);
	GetLcString("OtherStar",   &OtherStar);
}

StarParams gbuffer_star(shared_ptr<Object> arg)
{
	if (IsStarRemnant(arg->SpecClass)) // Transport luminosity from accretion disc when object is a star remnant with accretion disc
	{
		if (isinf(arg->LumBol)) { arg->LumBol = 0; }
		if (!arg->NoAccretionDisk)
		{
			if (isinf(arg->AccretionDisk.LumBol)) { arg->AccretionDisk.LumBol = 0; }
			arg->LumBol += arg->AccretionDisk.LumBol;
		}
	}
	using namespace cse::planet;
	return
	{
		.AbsMagnBol = ToAbsMagn4(arg->LumBol),
		.SpType = arg->SpecClass,
		.EqRadius = EquatorialRadius(*arg),
		.EqCircum = CircumferenceE(*arg),
		.Flattening = Flattening(*arg),
		.SurfArea = SurfaceArea(*arg),
		.Volume = Volume(*arg),
		.Mass = arg->Mass,
		.AvgDensity = MeanDensity(*arg),
		.EqSurfGrav = SurfaceGravity(*arg),
		.EscapeVelocity = EscapeVelocity(*arg),
		.Temperature = arg->Teff,
		.Luminosity = arg->LumBol,
		.Obliquity = arg->Rotation.Obliquity,
		.RotationPeriod = arg->Rotation.RotationPeriod
	};
}