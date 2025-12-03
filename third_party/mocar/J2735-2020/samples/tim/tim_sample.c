#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <v2x_api.h>
#include <mocar_log.h>

void tim_user_tim_recv_handle(v2x_msg_tim_t* usr_tim, void* param)
{
    MOCAR_LOG_ERROR("rxmsg-tim: message count \n");
}

int minute_of_the_year(struct tm* utc_time)
{
    int minute = 0;

    minute = (utc_time->tm_yday * 24 * 60) + (utc_time->tm_hour * 60) + utc_time->tm_min;
    return minute;
}

int tim_fill_usr_tim(v2x_msg_tim_t* usr_tim)
{
    if (NULL == usr_tim)
    {
        MOCAR_LOG_ERROR("usr_tim invaild parameter\n");
        return -1;
    }

    static uint8_t msgcount = 0;
	time_t time_now;
	struct tm* tm_time = NULL;
	time_now = time(NULL);
	tm_time = localtime(&time_now);

	usr_tim->msgCnt = msgcount % 128;
    msgcount++;

	usr_tim->timeStamp_active = SDK_OPTIONAL_EXSIT;
	usr_tim->timeStamp = minute_of_the_year(tm_time);
	usr_tim->packetID_active = SDK_OPTIONAL_EXSIT;
	usr_tim->packetID.size = 9;
	memcpy(usr_tim->packetID.buf, "BlackPink", usr_tim->packetID.size);
	usr_tim->urlB_active = SDK_OPTIONAL_EXSIT;
	usr_tim->urlB.size = 10;
	memcpy(usr_tim->urlB.buf, "Revolution", usr_tim->urlB.size);
	usr_tim->dataFrames_count = 3;
	for(uint8_t i = 0; i < usr_tim->dataFrames_count; i++)
	{
		Mde_TravelerDataFrame_t * tmp_data = &usr_tim->dataFrames[i];

		tmp_data->sspTimRights = 30;
		tmp_data->frameType = 3;
		tmp_data->present = Mde_TravelerDataFrame__msgId_PR_roadSignID;
		tmp_data->roadSignID.position.lat = 900000000;
		tmp_data->roadSignID.position.Long = 1800000000;
		tmp_data->roadSignID.position.elevation_active = SDK_OPTIONAL_EXSIT;
		tmp_data->roadSignID.position.elevation = 60000;
		tmp_data->roadSignID.viewAngle.size = MDE_HEADING_SLICE_BUFF_MAX;
		tmp_data->roadSignID.viewAngle.buf[3] = 1;
		tmp_data->roadSignID.viewAngle.buf[4] = 1;
		tmp_data->roadSignID.mutcdCode_active = SDK_OPTIONAL_EXSIT;
		tmp_data->roadSignID.mutcdCode = 5;
		tmp_data->roadSignID.crc_active = SDK_OPTIONAL_EXSIT;
		tmp_data->roadSignID.crc.size = 2;
		memcpy(tmp_data->roadSignID.crc.buf, "BP", tmp_data->roadSignID.crc.size);
		
		tmp_data->startYear_active = SDK_OPTIONAL_EXSIT;
		tmp_data->startYear = 1996;
		tmp_data->startTime = 1000;
		tmp_data->duratonTime = 2000;
		tmp_data->priority = 6;
		tmp_data->sspLocationRights = 30;
		tmp_data->GeographicalPath_count = 1;
		tmp_data->GeographicalPath[0].name_active = SDK_OPTIONAL_EXSIT;
		tmp_data->GeographicalPath[0].name.size = 10;
		memcpy(tmp_data->GeographicalPath[0].name.buf, "RegionName", tmp_data->GeographicalPath[0].name.size);
		tmp_data->GeographicalPath[0].id_active = SDK_OPTIONAL_EXSIT;
		tmp_data->GeographicalPath[0].id.region_active = SDK_OPTIONAL_EXSIT;
		tmp_data->GeographicalPath[0].id.region = 60000;
		tmp_data->GeographicalPath[0].id.id = 65000;
		tmp_data->GeographicalPath[0].anchor_active = SDK_OPTIONAL_EXSIT;
		tmp_data->GeographicalPath[0].anchor.lat = 900000000;
		tmp_data->GeographicalPath[0].anchor.Long = 1800000000;
		tmp_data->GeographicalPath[0].anchor.elevation_active = SDK_OPTIONAL_EXSIT;
		tmp_data->GeographicalPath[0].anchor.elevation = 60000;
		tmp_data->GeographicalPath[0].laneWidth_active = SDK_OPTIONAL_EXSIT;
		tmp_data->GeographicalPath[0].laneWidth = 32323;
		tmp_data->GeographicalPath[0].directionality_active = SDK_OPTIONAL_EXSIT;
		tmp_data->GeographicalPath[0].directionality = 2;
		tmp_data->GeographicalPath[0].closedPath_active = SDK_OPTIONAL_EXSIT;
		tmp_data->GeographicalPath[0].closedPath = 1;
		tmp_data->GeographicalPath[0].direction_active = SDK_OPTIONAL_EXSIT;
		tmp_data->GeographicalPath[0].direction.size = MDE_HEADING_SLICE_BUFF_MAX;
		tmp_data->GeographicalPath[0].direction.buf[1] = 1;
		tmp_data->GeographicalPath[0].direction.buf[3] = 1;
		tmp_data->GeographicalPath[0].description_active = SDK_OPTIONAL_EXSIT;
		if (i == 0)
		{
			tmp_data->GeographicalPath[0].description.present = Mde_GeographicalPath__description_PR_path;
			tmp_data->GeographicalPath[0].description.path.scale_active = SDK_OPTIONAL_EXSIT;
			tmp_data->GeographicalPath[0].description.path.scale = 14;
			tmp_data->GeographicalPath[0].description.path.present = Mde_OffsetSystem__offset_PR_xy;
			//others: Mde_OffsetSystem__offset_PR_ll
			tmp_data->GeographicalPath[0].description.path.xy.present = Mde_NodeListXY_PR_nodes;
			//others: Mde_NodeListXY_PR_computed
			tmp_data->GeographicalPath[0].description.path.xy.NodeSetXY_count = 2;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].delta.present = Mde_NodeOffsetPointXY_PR_node_XY2;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].delta.node_XY2.x = -1000;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].delta.node_XY2.y = 1000;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].attributes_active = SDK_OPTIONAL_EXSIT;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].attributes.localNode_count = 1;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].attributes.localNode[0] = 1;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].attributes.disabled_count = 2;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].attributes.disabled[0] = 2;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].attributes.disabled[1] = 2;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].attributes.enabled_count = 3;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].attributes.enabled[0] = 3;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].attributes.enabled[1] = 3;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].attributes.enabled[2] = 3;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].attributes.data_count = 1;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].attributes.data[0].present = Mde_LaneDataAttribute_PR_pathEndPointAngle;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].attributes.data[0].choice.pathEndPointAngle = 100;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].attributes.dWidth_active = SDK_OPTIONAL_EXSIT;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].attributes.dWidth = 500;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].attributes.dElevation_active = SDK_OPTIONAL_EXSIT;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[0].attributes.dElevation = 501;	
			tmp_data->GeographicalPath[0].description.path.xy.nodes[1].delta.present = Mde_NodeOffsetPointXY_PR_node_XY3;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[1].delta.node_XY3.x = -99;
			tmp_data->GeographicalPath[0].description.path.xy.nodes[1].delta.node_XY3.y = 99;
		}
		else if (i == 1)
		{
			tmp_data->GeographicalPath[0].description.present = Mde_GeographicalPath__description_PR_geometry;
			tmp_data->GeographicalPath[0].description.geometry.direction.size = 16;
			tmp_data->GeographicalPath[0].description.geometry.direction.buf[6] =1;
			tmp_data->GeographicalPath[0].description.geometry.direction.buf[8] = 1;
			tmp_data->GeographicalPath[0].description.geometry.extent_active = SDK_OPTIONAL_EXSIT;
			tmp_data->GeographicalPath[0].description.geometry.extent = 10;
			tmp_data->GeographicalPath[0].description.geometry.laneWidth_active = SDK_OPTIONAL_EXSIT;
			tmp_data->GeographicalPath[0].description.geometry.laneWidth = 32323;
			tmp_data->GeographicalPath[0].description.geometry.circle.center.lat = 900000000;
			tmp_data->GeographicalPath[0].description.geometry.circle.center.Long = 1800000000;
			tmp_data->GeographicalPath[0].description.geometry.circle.center.elevation_active = SDK_OPTIONAL_EXSIT;
			tmp_data->GeographicalPath[0].description.geometry.circle.center.elevation = 60000;
			tmp_data->GeographicalPath[0].description.geometry.circle.radius = 1000;
			tmp_data->GeographicalPath[0].description.geometry.circle.units = 4;
		}
		else 
		{
			tmp_data->GeographicalPath[0].description.present = Mde_GeographicalPath__description_PR_oldRegion;
			tmp_data->GeographicalPath[0].description.oldRegion.direction.size = MDE_HEADING_SLICE_BUFF_MAX;
			tmp_data->GeographicalPath[0].description.oldRegion.direction.buf[0] = 1;
			tmp_data->GeographicalPath[0].description.oldRegion.direction.buf[1] = 1;
			tmp_data->GeographicalPath[0].description.oldRegion.direction.buf[2] = 1;
			tmp_data->GeographicalPath[0].description.oldRegion.extent_active = SDK_OPTIONAL_EXSIT;
			tmp_data->GeographicalPath[0].description.oldRegion.extent = 9;
			tmp_data->GeographicalPath[0].description.oldRegion.area.present = Mde_ValidRegion__area_PR_shapePointSet;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.anchor_active = SDK_OPTIONAL_EXSIT;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.anchor.lat = 900000000;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.anchor.Long = 1800000000;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.anchor.elevation_active = SDK_OPTIONAL_EXSIT;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.anchor.elevation = 60000;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.laneWidth_active = SDK_OPTIONAL_EXSIT;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.laneWidth = 1010;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.directionality_active = SDK_OPTIONAL_EXSIT;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.directionality = 2;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.nodeList.present = Mde_NodeListXY_PR_computed;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.nodeList.computed.referenceLaneId = 200;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.nodeList.computed.present_x = Mde_ComputedLane__offsetXaxis_PR_small;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.nodeList.computed.small_x = 2000;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.nodeList.computed.present_y = Mde_ComputedLane__offsetYaxis_PR_small;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.nodeList.computed.small_y = 2001;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.nodeList.computed.rotateXY_active = SDK_OPTIONAL_EXSIT;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.nodeList.computed.rotateXY = 10;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.nodeList.computed.scaleXaxis_active = SDK_OPTIONAL_EXSIT;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.nodeList.computed.scaleXaxis = -10;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.nodeList.computed.scaleYaxis_active = SDK_OPTIONAL_EXSIT;
			tmp_data->GeographicalPath[0].description.oldRegion.area.shapePointSet.nodeList.computed.scaleYaxis = -11;
		}
			
		tmp_data->sspMsgRights1 = 15;
		tmp_data->sspMsgRights2 = 30;
		tmp_data->content.present = Mde_TravelerDataFrame__content_PR_advisory;
		tmp_data->content.advisory_count = 2;
		tmp_data->content.advisory[0].present = Mde_ITIScodesAndText__Member__item_PR_itis;
		tmp_data->content.advisory[0].itis = 2;
		tmp_data->content.advisory[1].present = Mde_ITIScodesAndText__Member__item_PR_text;
		tmp_data->content.advisory[1].text.size = 15;
		memcpy(tmp_data->content.advisory[1].text.buf, "Love sick girl.", tmp_data->content.advisory[1].text.size);

		tmp_data->url_active = SDK_OPTIONAL_EXSIT;
		tmp_data->url.size = 9;
		memcpy(tmp_data->url.buf, "boombayah", tmp_data->url.size);

	}
		
	return 0;
}

void tim_tim_tx(void)
{
    int ret         = -1;

	v2x_msg_tim_t * usr_tim = calloc(1, sizeof(v2x_msg_tim_t));
	if (NULL == usr_tim)
	{
		MOCAR_LOG_ERROR("usr_tim calloc fail");
		return;
	}
	
    ret = tim_fill_usr_tim(usr_tim);
    if(0 != ret)
    {
        MOCAR_LOG_ERROR("user tim fill fail\n");
		free(usr_tim);
		usr_tim = NULL;
        return;
    }
    MOCAR_LOG_INFO("tim fill success!");
    ret = mde_v2x_tim_send(usr_tim, NULL, 32);
    if(0 != ret)
    {
        MOCAR_LOG_ERROR("tim send fail\n");
		free(usr_tim);
		usr_tim = NULL;
        return;   
    }
	
	free(usr_tim);
	usr_tim = NULL;

    fprintf(stderr, "txmsg-tim: send msg successed, sequence \n");
	MOCAR_LOG_INFO("txmsg-tim: send msg successed, sequence \n");
}

int main(int argc, char* argv[])
{
    int ret = -1;

    ret = mde_v2x_init(0);
    if(0 != ret)
    {
        printf("cv2x init fail\n");
        return -1;    
    }

    //mde_v2x_tim_recv_handle_register(tim_user_tim_recv_handle); 
    //printf("tim recv handle register \n");
    
    while(1)
    {
        tim_tim_tx();
        usleep(1000*1000);
    }
    
    return 0;
}
