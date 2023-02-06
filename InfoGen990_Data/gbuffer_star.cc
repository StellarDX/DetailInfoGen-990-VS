#include "gbuffer_star.h"

using namespace std;
using namespace cse;

string GenStarType(shared_ptr<Object> Pointer)
{
	SPECSTR Spec = Pointer->SpecClass;
	if (IsHyperGiant(Spec)){return "Hypergiant";}
	if (IsSuperGiant(Spec))
	{
		if (IsOType(Spec) || IsBType(Spec) || IsAType(Spec)){return "BSG";}
		if (IsFType(Spec) || IsGType(Spec)) { return "YSG"; }
		if (IsKType(Spec) || IsMType(Spec) || IsLType(Spec)) { return "RSG"; }
		return "Supergiant";
	}
	if (IsGiant(Spec))
	{
		if (IsOType(Spec) || IsBType(Spec) || IsAType(Spec)) { return "Blue giant"; }
		if (IsFType(Spec) || IsGType(Spec)) { return "Yellow giant"; }
		if (IsKType(Spec) || IsMType(Spec)) { return "Red giant"; }
		return "Giant star";
	}
	if (IsSubGiant(Spec)) { return "Subgiant"; }
	if (IsMainSequence(Spec))
	{
		if (IsOType(Spec)) { return "O-type main-sequance star"; }
		if (IsBType(Spec)) { return "B-type main-sequance star"; }
		if (IsAType(Spec)) { return "A-type main-sequance star"; }
		if (IsFType(Spec)) { return "F-type main-sequance star"; }
		if (IsGType(Spec)) { return "Yellow dwarf"; }
		if (IsKType(Spec)) { return "Orange dwarf"; }
		if (IsMType(Spec)) { return "Red dwarf"; }
		return "Main-Sequence";
	}
	if (IsSubDwarf(Spec))
	{
		if (IsOType(Spec)) { return "sdO"; }
		if (IsBType(Spec)) { return "sdB"; }
		return "Subdwarf";
	}
	if (IsWolfRayet(Spec)) { return "Wolf-Rayet Star"; }
	if (IsBrownDwarf(Spec))
	{
		if (IsLType(Spec)) { return "L-type dwarf"; }
		if (IsTType(Spec)) { return "Brown dwarf"; }
		if (IsYType(Spec)) { return "Sub-brown dwarf"; }
		return "Substellar object";
	}
	if (IsCarbonStar(Spec))
	{
		if (Spec.SClass() >= 28 && Spec.SClass() <= 30) { return "S-type star"; }
		return "Carbon star";
	}
	if (IsWhiteDwarf(Spec)) { return "White dwarf"; }
	if (IsNeutronStar(Spec)) { return "Pulsar"; }
	if (IsBlackHole(Spec))
	{
		if (isinf(Pointer->KerrSpin)) { Pointer->KerrSpin = 0; }
		if (isinf(Pointer->KerrCharge)) { Pointer->KerrCharge = 0; }
		if (Pointer->KerrSpin == 0 && Pointer->KerrCharge == 0) { return "Schwarzschild Black hole"; }
		if (Pointer->KerrSpin != 0 && Pointer->KerrCharge == 0) { return "Kerr Black hole"; }
		if (Pointer->KerrSpin == 0 && Pointer->KerrCharge != 0) { return "Reissner-Nordstrom Black hole"; }
		if (Pointer->KerrSpin != 0 && Pointer->KerrCharge != 0) { return "Kerr-Newman Black hole"; }
		return "Black hole";
	}
	return "Star";
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

StarParams gbuffer_star(shared_ptr<Object> arg)
{
	float64 MaxRadius = max(arg->Dimensions.x, arg->Dimensions.z) / 2.;
	float64 Vol = float64((4. / 3.) * CSE_PI * (arg->Dimensions.x / 2.) * (arg->Dimensions.y / 2.) * (arg->Dimensions.z / 2.));
	if (IsStarRemnant(arg->SpecClass)) // Transport luminosity from accretion disc when object is a star remnant with accretion disc
	{
		if (isinf(arg->LumBol)) { arg->LumBol = 0; }
		if (!arg->NoAccretionDisk)
		{
			if (isinf(arg->AccretionDisk.LumBol)) { arg->AccretionDisk.LumBol = 0; }
			arg->LumBol += arg->AccretionDisk.LumBol;
		}
	}
	return
	{
		.AbsMagnBol = ToAbsMagn4(arg->LumBol),
		.SpType = arg->SpecClass,
		.EqRadius = MaxRadius,
		.Flattening = (MaxRadius - (arg->Dimensions.y / 2.)) / MaxRadius,
		.Volume = Vol,
		.Mass = arg->Mass,
		.AvgDensity = arg->Mass / Vol,
		.EqSurfGrav = (GravConstant * arg->Mass) / cse::pow(MaxRadius, 2),
		.EscapeVelocity = cse::sqrt((2. * GravConstant * arg->Mass) / MaxRadius),
		.Temperature = arg->Teff,
		.Luminosity = arg->LumBol,
		.Obliquity = arg->Rotation.Obliquity,
		.RotationPeriod = arg->Rotation.RotationPeriod
	};
}