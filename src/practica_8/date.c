
unsigned int date2Int(struct DATE date)
{
  unsigned int val = 0;

  val = date.year - 1970;
  val <<= 4;
  val |= date.month;
  val <<= 5;
  val |= date.day;
  val <<= 5;
  val |= date.hour;
  val <<= 6;
  val |= date.min;
  val <<= 6;
  val |= date.sec;

  return val;
}

int int2Date(struct DATE *date, unsigned int val)
{
  date -> sec = val & 0x3F;
  val >>= 6;
  date -> min = val & 0x3F;
  val >>= 6;
  date -> hour = val & 0x1F;
  val >>= 5;
  date -> day = val & 0x1F;
  val >>= 5;
  date -> month = val & 0x0F;
  val >>= 4;
  date -> year = (val & 0x3F) + 1970;

  return SUCCESS;
}

unsigned int currDateTime2Int()
{
  struct tm *tm_ptr;
  time_t the_time;

  struct DATE date;

  (void) time(&the_time);
  tm_ptr = gmtime(&the_time);

  date.year = tm_ptr -> tm_year - 70;
  date.month = tm_ptr -> tm_mon + 1;
  date.day = tm_ptr -> tm_mday;
  date.hour = tm_ptr -> tm_hour;
  date.min = tm_ptr -> tm_min;
  date.sec = tm_ptr -> tm_sec;
  return datetoint(date) ;
}