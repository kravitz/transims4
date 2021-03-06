//*********************************************************
//	Read_Trip.cpp - reads the trip file
//*********************************************************

#include "ArcTraveler.hpp"

#include "Trip_File.hpp"
#include "In_Polygon.hpp"

//---------------------------------------------------------
//	Read_Trip
//---------------------------------------------------------

void ArcTraveler::Read_Trip (void)
{
	int hhold, person, last_hh, last_person, time, nsaved;
	bool save_flag, keep_flag, select_flag;

	Trip_File *file = (Trip_File *) Demand_Db_Base (TRIP);

	XYZ_Point pt;
	Link_Data *link_ptr = NULL;
	Location_Data *location_ptr = NULL;

	Show_Message ("Reading %s -- Record", file->File_Type ());
	Set_Progress (10000);
	Print (1);
	
	select_flag = (travelers.Num_Ranges () > 0);
	keep_flag = true;

	last_hh = last_person = nsaved = 0;
	save_flag = false;

	//---- read the trip file ----

	while (file->Read ()) {
		Show_Progress ();

		hhold = file->Household ();
		person = file->Person ();

		if (hhold != last_hh || person != last_person) {
			if (save_flag) {
				if (!arcview_traveler.Write_Record ()) {
					Error ("Writing ArcView Traveler File");
				}
				nsaved++;
			}
			save_flag = false;
			last_hh = hhold;
			last_person = person;

			if (select_flag) {
				keep_flag = travelers.In_Range (hhold * Traveler_Scale () + person);
				if (!keep_flag) continue;
			}
			arcview_traveler.Copy_Fields (file);
			arcview_traveler.points.Reset ();
		}
		if (!keep_flag) continue;

		//---- check the start time ----

		if (time_flag) {
			time = times.Step (file->Start ());
			if (!times.In_Range (time)) continue;
		}

		//---- get the origin ----

		location_ptr = location_data.Get (file->Origin ());
		if (location_ptr == NULL) continue;

		//---- check the subarea ----

		if (subarea_flag) {
			if (!In_Polygon (UnRound (location_ptr->X ()), UnRound (location_ptr->Y ()), &select_subarea.points)) continue;
		}

		//---- save the coordinates ----

		pt.x = UnRound (location_ptr->X ());
		pt.y = UnRound (location_ptr->Y ());
		pt.z = 0.0;

		arcview_traveler.points.Add (&pt);

		//---- check the arrival time ----

		if (time_flag) {
			time = times.Step (file->Arrive ());
			if (!times.In_Range (time)) continue;
		}

		//---- get the destination ----

		location_ptr = location_data.Get (file->Destination ());
		if (location_ptr == NULL) continue;

		//---- check the subarea ----

		if (subarea_flag) {
			if (!In_Polygon (UnRound (location_ptr->X ()), UnRound (location_ptr->Y ()), &select_subarea.points)) continue;
		}

		//---- save the coordinates ----

		pt.x = UnRound (location_ptr->X ());
		pt.y = UnRound (location_ptr->Y ());

		arcview_traveler.points.Add (&pt);

		save_flag = true;
	}
	End_Progress ();

	if (save_flag) {
		if (!arcview_traveler.Write_Record ()) {
			Error ("Writing ArcView Traveler File");
		}
		nsaved++;
	}

	file->Close ();
	arcview_traveler.Close ();

	Print (1, "Number of Trip Records Read = %d", Progress_Count ());
	Print (1, "Number of Traveler Records Saved = %d", nsaved);
}

