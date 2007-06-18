<?php
// $Id$

/**
 * Verifies that $phonenumber is a valid ten-digit North American phone number
 *
 * @param string $phonenumber
 * @return boolean Returns boolean FALSE if the phone number is not valid. 
 */

function valid_ca_phone_number($phonenumber) {

  //$phonenumber = trim($phonenumber);

  // define regular expression
  $regex = "/
    \D*           # ignore non-digits
    1?            # an optional 1
    \D*           # optional separator 
    [02-9]\d{2}   # area code (can't start with 1)
    \D*           # optional separator
    \d{3}         # 3-digit prefix
    \D*           # optional separator
    \d{4}         # 4-digit line number
    \D*           # optional separator
    \d*           # optional extension
    \D*           # ignore trailing non-digits
    /x";
  // return true if valid, false otherwise
  return (bool) preg_match($regex, $phonenumber); 
} 

/**
 * Convert a valid North American phone number into standard (444) 867-5309 x1234 format
 * 
 * @param $phonenumber must be a valid ten-digit number (with optional extension)
 * 
 */
function format_ca_phone_number($phonenumber) {

  // define regular expression   
  $regex = "/
    ^\D*          # ignore non-digits
    1?            # an optional 1
    \D*           # optional separator 
    ([2-9]\d{2})  # capture area code       
    \D*           # optional separator
    ([2-9]\d{2})  # capture 3-digit prefix
    \D*           # optional separator
    (\d{4})       # capture 4-digit line number                       
    \D*           # optional separator
    (\d*)         # capture optional extension                       
    \D*$          # ignore trailing non-digits
  /x"; 
    
  // get digits of phone number
  preg_match($regex, $phonenumber, $matches);
  
  // construct ten-digit phone number
  $phonenumber = $matches[1] . '-' . $matches[2] . '-' . $matches[3];
  
  if ($matches[1] != "1") {
  	$phonenumber = "1" . " " . $phonenumber; 
  }
  return $phonenumber;
}