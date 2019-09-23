#include "Dependencies.h"

#ifdef TEST_DEPENDENCIES

// ----< test stub for Dependencies package >---------------------------
int main(int argc, char ** argv) {

	Dependencies dt;
	
	dt.addDependency("A.h", "B.h");
	dt.addDependency("A.h", "C.h");
	dt.addDependency("C.h", "B.h");
	dt.addDependency("D.h", "B.h");
	dt.addDependency("E.h", "A.h");
	dt.addDependency("B.h", "E.h");
	dt.addDependency("C.h", "E.h");
	dt.addDependency("E.h", "E.h");
	dt.removeDependency("E.h", "E.h");
	dt.display();

	return 0;

}

#endif
