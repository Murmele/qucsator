/*
 * cpwstep.h - coplanar waveguide step class definitions
 *
 * Copyright (C) 2005 Stefan Jahn <stefan@lkcc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * $Id: cpwstep.h,v 1.1 2005/05/12 17:38:46 raimi Exp $
 *
 */

#ifndef __CPWSTEP_H__
#define __CPWSTEP_H__

class cpwstep : public circuit
{
 public:
  cpwstep ();
  void initSP (void);
  void calcSP (nr_double_t);
  void initDC (void);
  void initAC (void);
  void calcAC (nr_double_t);

  void checkProperties (void);
  void calcCends (nr_double_t, nr_double_t&, nr_double_t&);
  complex calcY (nr_double_t);
};

#endif /* __CPWSTEP_H__ */
