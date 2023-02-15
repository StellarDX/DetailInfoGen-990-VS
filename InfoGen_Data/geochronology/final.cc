#include "composite.h"
#include "final.h"

#include <CSE/Object.h>

using namespace std;
using namespace cse;

ISCStream SysIn;

void geochronology(ISCStream& SystemIn, vector<string> args)
{
	if (find(args.begin(), args.end(), "-target") == args.end())
	{
		cout << "Object is unknown.\n";
		abort();
	}

	string ObjectName, ParentName;
	SysIn = SystemIn;
	
	// Find ObjectName and ParentName
	for (size_t i = 0; i < args.size(); i++)
	{
		if (args[i] == "-target" && i < args.size())
		{
			if (args[i + 1][0] == '-' || i == args.size() - 1)
			{
				cout << "Invalid object name." << '\n';
				abort();
			}
			ObjectName = args[i + 1];
			break;
		}
		else if (args[i] == "-parent" && i < args.size())
		{
			if (args[i + 1][0] == '-' || i == args.size() - 1)
			{
				cout << "Invalid parent name." << '\n';
				abort();
			}
			ParentName = args[i + 1];
			break;
		}
	}

	// Find object

	Object Target = GetSEObject(SystemIn, ObjectName);
	Object Parent;
	if (ParentName.empty()) { Parent = GetSEObject(SystemIn, Target.ParentBody); }
	else { Parent = GetSEObject(SystemIn, ParentName); }

	if (!(Target.Type != "Jupiter" && Target.Type != "GasGiant" && Target.Type != "Neptune" && Target.Type != "IceGiant") &&
		Target.NoOcean && Target.NoAtmosphere &&
		!(Target.Life[0].Class == "Organic" && Target.Life[0].Type == "Multicellular") &&
		!(Target.Life[1].Class == "Organic" && Target.Life[1].Type == "Multicellular"))
	{
		cout << Target.Name[0] + " is not match the requirements.\n";
		abort();
	}

	composite0geo(Target, Parent);
}