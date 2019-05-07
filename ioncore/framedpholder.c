/*
 * notion/ioncore/framedpholder.c
 *
 * Copyright (c) the Notion team 2013. 
 * Copyright (c) Tuomo Valkonen 2005-2007. 
 *
 * See the included file LICENSE for details.
 */

#include <libtu/objp.h>
#include <libtu/obj.h>
#include <libtu/minmax.h>

#include "frame.h"
#include "framedpholder.h"
#include "sizehint.h"


/*{{{ Init/deinit */


bool framedpholder_init(WFramedPHolder *ph, WPHolder *cont,
                        const WFramedParam *param)
{
    assert(cont!=NULL);
    
    pholder_init(&(ph->ph));

    ph->cont=cont;
    ph->param=*param;
    
    return TRUE;
}
 

WFramedPHolder *create_framedpholder(WPHolder *cont,
                                     const WFramedParam *param)
{
    CREATEOBJ_IMPL(WFramedPHolder, framedpholder, (p, cont, param));
}


void framedpholder_deinit(WFramedPHolder *ph)
{
    if(ph->cont!=NULL){
        destroy_obj((Obj*)ph->cont);
        ph->cont=NULL;
    }
    
    pholder_deinit(&(ph->ph));
}


/*}}}*/


/*{{{ Attach */


typedef struct{
    WRegionAttachData *data;
    WFramedParam *param;
} AP;


void frame_adjust_to_initial(WFrame *frame, const WFitParams *fp, 
                             const WFramedParam *param, WRegion *reg)
{
    WRectangle rqg, mg;
 
    if(!(fp->mode&(REGION_FIT_BOUNDS|REGION_FIT_WHATEVER)))
        return;

    mplex_managed_geom((WMPlex*)frame, &mg);

    /* Adjust geometry */
    if(!param->inner_geom_gravity_set){
        rqg.x=REGION_GEOM(frame).x;
        rqg.y=REGION_GEOM(frame).y;
        rqg.w=MAXOF(1, REGION_GEOM(reg).w+(REGION_GEOM(frame).w-mg.w));
        rqg.h=MAXOF(1, REGION_GEOM(reg).h+(REGION_GEOM(frame).h-mg.h));
    }else{
        int bl=mg.x;
        int br=REGION_GEOM(frame).w-(mg.x+mg.w);
        int bt=mg.y;
        int bb=REGION_GEOM(frame).h-(mg.y+mg.h);
        
        rqg.x=(/*fp->g.x+*/param->inner_geom.x+
               xgravity_deltax(param->gravity, bl, br));
        rqg.y=(/*fp->g.y+*/param->inner_geom.y+
               xgravity_deltay(param->gravity, bt, bb));
        rqg.w=MAXOF(1, param->inner_geom.w+(REGION_GEOM(frame).w-mg.w));
        rqg.h=MAXOF(1, param->inner_geom.h+(REGION_GEOM(frame).h-mg.h));
    }

    if(!(fp->mode&REGION_FIT_WHATEVER))
        rectangle_constrain(&rqg, &fp->g);
    
    region_fit((WRegion*)frame, &rqg, REGION_FIT_EXACT);
}


WRegion *framed_handler(WWindow *par, 
                        const WFitParams *fp, 
                        void *ap_)
{
    AP *ap=(AP*)ap_;
    WMPlexAttachParams mp=MPLEXATTACHPARAMS_INIT;
    WFramedParam *param=ap->param;
    WFrame *frame;
    WRegion *reg;
    
    frame=create_frame(par, fp, param->mode, "Framed PHolder Frame");

    if(frame==NULL)
        return NULL;
    
    if(fp->mode&(REGION_FIT_BOUNDS|REGION_FIT_WHATEVER))
        mp.flags|=MPLEX_ATTACH_WHATEVER;

    reg=mplex_do_attach(&frame->mplex, &mp, ap->data);
    
    if(reg==NULL){
        destroy_obj((Obj*)frame);
        return NULL;
    }
    
    frame_adjust_to_initial(frame, fp, param, reg);
    
    return (WRegion*)frame;
}


WRegion *region_attach_framed(WRegion *reg, WFramedParam *param,
                              WRegionAttachFn *fn, void *fn_param,
                              WRegionAttachData *data)
{
    WRegionAttachData data2;
    AP ap;

    data2.type=REGION_ATTACH_NEW;
    data2.u.n.fn=framed_handler;
    data2.u.n.param=&ap;
    
    ap.data=data;
    ap.param=param;
    
    return fn(reg, fn_param, &data2);
}


WRegion *framedpholder_do_attach(WFramedPHolder *ph, int flags,
                                 WRegionAttachData *data)
{
    WRegionAttachData data2;
    AP ap;
    
    if(ph->cont==NULL)
        return FALSE;

    data2.type=REGION_ATTACH_NEW;
    data2.u.n.fn=framed_handler;
    data2.u.n.param=&ap;
    
    ap.data=data;
    ap.param=&ph->param;
        
    return pholder_do_attach(ph->cont, flags, &data2);
}


/*}}}*/


/*{{{ Other dynfuns */


bool framedpholder_do_goto(WFramedPHolder *ph)
{
    return (ph->cont!=NULL
            ? pholder_goto(ph->cont)
            : FALSE);
}


WRegion *framedpholder_do_target(WFramedPHolder *ph)
{
    return (ph->cont!=NULL
            ? pholder_target(ph->cont)
            : NULL);
}


WPHolder *framedpholder_do_root(WFramedPHolder *ph)
{
    WPHolder *root;
    
    if(ph->cont==NULL)
        return NULL;
    
    root=pholder_root(ph->cont);
    
    return (root!=ph->cont 
            ? root
            : &ph->ph);
}


/*}}}*/


/*{{{ Class information */


static DynFunTab framedpholder_dynfuntab[]={
    {(DynFun*)pholder_do_attach, 
     (DynFun*)framedpholder_do_attach},

    {(DynFun*)pholder_do_goto, 
     (DynFun*)framedpholder_do_goto},

    {(DynFun*)pholder_do_target, 
     (DynFun*)framedpholder_do_target},
     
    {(DynFun*)pholder_do_root, 
     (DynFun*)framedpholder_do_root},
    
    END_DYNFUNTAB
};

IMPLCLASS(WFramedPHolder, WPHolder, framedpholder_deinit, 
          framedpholder_dynfuntab);


/*}}}*/

