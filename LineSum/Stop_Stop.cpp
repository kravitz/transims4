//*********************************************************
//	Stop_Stop.cpp - Create a Stop Group Report by Stop
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Stop_Stop_Report
//---------------------------------------------------------

void LineSum::Stop_Stop_Report (void)
{
	int on_bus, on_rail, off_bus, off_rail;
	int tot_on_bus, tot_on_rail, tot_off_bus, tot_off_rail;

	int i, j, stop, stops, run, runs, num, time, on, off;

	Rider_Data *rider_ptr;
	Integer_List *group;
	Stop_Data *stop_ptr;

	Show_Message ("Creating Transit Stop Group by Stop -- Record");
	Set_Progress (100);

	//---- initialize the selection sets ----

	set_ptr = select_set.First ();
	if (set_ptr == NULL) return;

	Header_Number (STOP_STOP);

	num = stop_equiv.Max_Group ();

	//---- process each stop group ----

	for (i=1; i <= num; i++) {

		group = stop_equiv.Group_List (i);
		if (group == NULL) continue;
		
		group_num = i;

		if (!Break_Check (group->Num_Records () + 8)) {
			Print (1);
			Stop_Stop_Header ();
		}
		tot_on_bus = tot_on_rail = tot_off_bus = tot_off_rail = 0;

		//---- process each stop ----

		for (stop = group->First (); stop != 0; stop = group->Next ()) {
			Show_Progress ();

			stop_ptr = stop_data.Get (stop);
			if (stop_ptr == NULL) continue;

			on_bus = on_rail = off_bus = off_rail = 0;

			//---- process each route ----

			for (rider_ptr = rider_data.First_Key (); rider_ptr; rider_ptr = rider_data.Next_Key ()) {

				//---- check the selection criteria ----

				if (!set_ptr->Select_Modes (rider_ptr->Mode ())) continue;
				if (!set_ptr->Select_Routes (rider_ptr->Route ())) continue;

				stops = rider_ptr->Stops ();
				runs = rider_ptr->Runs ();

				//---- check the stop ----

				for (j=1; j <= stops; j++) {
					if (stop != rider_ptr->Stop (j)) continue;

					//---- process each run ----

					for (run=1; run <= runs; run++) {
						switch (set_ptr->Time_Method ()) {
							case RUN_START:
							case RUN_END:
							case RUN_MID:
								time = rider_ptr->Time (run, j);
								break;
							case SCHED_START:
							case SCHED_END:
							case SCHED_MID:
								time = rider_ptr->Schedule (run, j);
								break;
						}
						if (set_ptr->Time_Period (Resolve (time)) == 0) continue;;

						on = rider_ptr->Board (run, j);
						off = rider_ptr->Alight (run, j);

						if (rider_ptr->Mode () > EXPRESS_BUS) {
							on_rail += on;
							off_rail += off;
						} else {
							on_bus += on;
							off_bus += off;
						}
					}
				}
			}

			//---- print the stop data ---

			Print (1, "%-6d %-17.17s%7d%7d%7d %7d%7d%7d %7d%7d%7d", 
				stop, stop_ptr->Name (), on_bus, on_rail, (on_bus + on_rail),
				off_bus, off_rail, (off_bus + off_rail), (on_bus + off_bus),
				(on_rail + off_rail), (on_bus + off_bus + on_rail + off_rail));

			tot_on_bus += on_bus;
			tot_on_rail += on_rail;
			tot_off_bus += off_bus;
			tot_off_rail += off_rail;
		}

		//---- print the total ---

		Print (2, "Total                   %7d%7d%7d %7d%7d%7d %7d%7d%7d", 
			tot_on_bus, tot_on_rail, (tot_on_bus + tot_on_rail),
			tot_off_bus, tot_off_rail, (tot_off_bus + tot_off_rail), 
			(tot_on_bus + tot_off_bus), (tot_on_rail + tot_off_rail), 
			(tot_on_bus + tot_off_bus + tot_on_rail + tot_off_rail));

	}
	End_Progress ();

	Header_Number (0);
}

//---------------------------------------------------------
//	Stop_Stop_Header
//---------------------------------------------------------

void LineSum::Stop_Stop_Header (void)
{
	Print (1, "Transit Stop Group by Stop Report -- %s", stop_equiv.Group_Label (group_num));

	Print (2, "%26c----- Boardings ----  ---- Alightings ----  ------ Total -------", BLANK);
	Print (1, "Stop   Name                 Bus   Rail  Total     Bus   Rail  Total     Bus   Rail  Total");
	Print (1);
}

/*********************************************|***********************************************

	Transit Stop Group by Stop Report 

	                         ----- Boardings ----  ---- Alightings ----  ------ Total -------
	Stop   Name                 Bus   Rail  Total     Bus   Rail  Total     Bus   Rail  Total

	dddddd sssssssssssssssss dddddd dddddd dddddd  dddddd dddddd dddddd  dddddd dddddd dddddd

	Total                    dddddd dddddd dddddd  dddddd dddddd dddddd  dddddd dddddd dddddd

**********************************************|***********************************************/ 
