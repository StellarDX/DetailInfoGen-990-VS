#pragma once

#ifndef __GEO_COMMON__
#define __GEO_COMMON__

// Timeline strings of eons, in Earth's model

// Reference:
// https://en.wikipedia.org/wiki/History_of_Earth
// https://en.wikipedia.org/wiki/Timeline_of_the_evolutionary_history_of_life#Detailed_timeline (Strings are taken from this link, CC BY-SA 3.0 Lic.)
// https://en.wikipedia.org/wiki/History_of_technology
// https://en.wikipedia.org/wiki/Timeline_of_historic_inventions (Strings are taken from this link, CC BY-SA 3.0 Lic.)

// This file is available under the Creative Commons Attribution-ShareAlike License 4.0;
// https://creativecommons.org/licenses/

static const std::string __Hadean_Eon_Events[] = 
{
	"Planet \"{0}\" forms from the accretion disc revolving around the star \"{1}\".",
	"Moons of \"{0}\" originated, gravitational pull of them stabilised \"{0}\"'s fluctuating axis of rotation, setting up regular climatic conditions favoring abiogenesis.",
	"Apperence of the first liquid water on \"{0}\".",
	"Earliest possible appearance of life on \"{0}\"."
};

static const std::string __Archean_Eon_Events[] =
{
	"Cells resembling prokaryotes appear. These first organisms are believed to have been chemoautotrophs, using carbon dioxide as a carbon source and oxidizing inorganic materials to extract energy.",
	"Bacteria and archaea apperar. Bacteria develop primitive photosynthesis, which at first did not produce oxygen. These organisms exploit a proton gradient to generate adenosine triphosphate (ATP), a mechanism used by virtually all subsequent organisms.",
	"Photosynthesizing cyanobacteria using water as a reducing agent and producing oxygen as a waste product. Free oxygen initially oxidizes dissolved iron in the oceans, creating iron ore. Oxygen concentration in the atmosphere slowly rises, poisoning many bacteria and eventually triggering the Great Oxygenation Event.",
};

static const std::string __Proterozoic_Eon_Events[] =
{
	"Great Oxidation Event led by cyanobacteria's oxygenic photosynthesis.",
	"Prokaryotes engulfing each other via phagocytosis and produce Eukaryotic cells.",
	"First non-marine eukaryotes move onto land.",
	"Beginning of animal evolution.",
	"Cambrian explosion. The first large, complex aquatic multicellular organisms appear.",
};

static const std::string __Palaeozoic_Era_Events[] =
{
	"**[Cambrian Period]**",
	"Major diversification of living things in the oceans.",
	"The first known footprints on land.",
	"**[Ordovician Period]**",
	"First vertebrates with true bones.",
	"**[Silurian Period]**",
	"First agnathan fishes.",
	"Earliest ray-finned fishes.",
	"**[Devonian Period]**",
	"First lichens, stoneworts.",
	"Some species begin to show some features similar to early tetrapods.",
	"By the start of the Carboniferous Period, the \"{0}\" begins to resemble its present state. Insects roamed the land and would soon take to the skies; sharks swam the oceans as top predators, and vegetation covered the land, with seed-bearing plants and forests soon to flourish. Four-limbed tetrapods gradually gain adaptations which will help them occupy a terrestrial life-habit.",
	"Land flora dominated by seed ferns.",
	"**[Carboniferous Period]**",
	"Diversification of amphibians.",
	"First Reptiliomorpha.",
	"First amniote vertebrates",
	"The Carboniferous rainforest collapse occurs, causing a minor extinction event, as well as paving the way for amniotes to become dominant over amphibians and seed plants over ferns and lycophytes. First diapsid reptiles.",
	"**[Permian Period]**",
	"Seed plants and conifers diversify while lepidodendrids and sphenopsids decrease. Terrestrial temnospondyl amphibians and pelycosaurs diversify in species.",
	"The Permian–Triassic extinction event eliminates over 90-95% of marine species."
};

static const std::string __Mesozoic_Era_Events[] =
{
	"**[Triassic Period]**",
	"Mesozoic marine revolution begins: increasingly well adapted and diverse predators stress sessile marine groups; the \"balance of power\" in the oceans shifts dramatically as some groups of prey adapt more rapidly and effectively than others.",
	"Earliest ichthyosaurs",
	"Earliest dinosaurs, First teleost fishes and First mammals appear.",
	"Seed-producing Gymnosperm forests dominate the land; herbivores grow to huge sizes; First mammals from small-sized cynodonts, mammals transitioned towards a nocturnal, insectivorous, and endothermic lifestyle.",
	"Massive Triassic/Jurassic extinction.",
	"**[Jurassic Period]**",
	"First pterosaurs with specialized feeding. First sauropod dinosaurs. Diversification in small, ornithischian dinosaurs.",
	"Sauropod dinosaurs diversify.",
	"Ancestor to the birds appear",
	"**[Cretaceous Peropd]**",
	"Rise of the angiosperms. Some of these flowering plants bear structures that attract insects and other animals to spread pollen; other angiosperms are pollinated by wind or water. This innovation causes a major burst of animal coevolution.",
	"Extinction of ichthyosaurs. Large diversification in angiosperms. Origins of placental mammals"
};

static const std::string __Cenozoic_Era_Events[] =
{
	"**[Paleogene Period]**",
	"The Cretaceous–Paleogene extinction event eradicates about half of all animal species, including mosasaurs, pterosaurs, plesiosaurs, ammonites, belemnites, rudist and inoceramid bivalves, most planktic foraminifers, and all of the dinosaurs excluding the birds.",
	"Rapid dominance of conifers and ginkgos in high latitudes, along with mammals becoming the dominant species.",
	"Diversification of large, flightless birds. Mammals begin to show intelligence.",
	"Modern bird groups diversify. Flowering plants continue to diversify. Ungulates split into artiodactyla and perissodactyla, with some members of the former returning to the sea.",
	"Diversification of intelligent mammals",
	"Grasses diversify from among the monocot angiosperms; grasslands begin to expand.",
	"**[Neogene Period]**",
	"Grasslands and savannas are established, diversity in insects, major diversification in grassland mammals and snakes.",
	"First hominins.",
	"Hominins evolves.",
	"**[First civilizations]**"
};

static const std::string __Civilization_Events[] =
{
	"Paleolithic - First stone tools.",
	std::vformat("**[Quaternary Period]** - at {} years since the planet forms.", std::make_format_args(4537420000)),
	"Mesolithic - Appearance of tools made for purely intellectual or artistic pursuits.",
	"Neolithic - stone remained the predominant material for toolmaking, although copper and arsenic bronze were developed towards the end of this period.",
	"Bronze Age - The beginning of bronze-smelting.",
	"Iron Age - The Late Bronze Age collapse occurs, followed by the beginning of the Iron Age.",
	"1st Industrial Revolution - Mechanized textile production, First Steam engine, Factory system, Iron production increase.",
	"2nd Industrial Revolution - The enormous expansion of rail and telegraph lines culminated in a new wave of globalization. First electrical power and telephones.",
	"3rd Industrial Revolution - Widespread use of digital logic, MOSFETs (MOS transistors), integrated circuit (IC) chips, and their derived technologies, including computers, microprocessors, digital cellular phones, and the Internet.",
	"4th Industrial Revolution - ..."
};

#endif