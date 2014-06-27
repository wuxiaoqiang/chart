

typedef struct tagQUERY_PEER_RES_CMD
{
    _u32    _protocol_version;
    _u32    _seq;
    _u32    _cmd_len;
    _u8     _cmd_type;
    _u32    _peerid_size;
    char        _peerid[PEER_ID_SIZE + 1];
    _u32    _cid_size;
    _u8     _cid[CID_SIZE];
    _u64    _file_size;
    _u32    _gcid_size;
    _u8     _gcid[CID_SIZE];
    _u32    _peer_capability;
    _u32    _internal_ip;
    _u32    _nat_type;
    _u8     _level_res_num;
    _u8     _resource_capability;   /* 0:普通查询   1:仅需要发行Server    2:仅需要外网资源  3:仅需要内网资源   4. 需要所有种类资源(与0的差别在于如果有cdn资源必须返回) */
    _int32  _server_res_num;
    _u32    _query_times;
    _u32    _p2p_capability;
    _u32    _upnp_ip;
    _u16    _upnp_port;
    _u8     _rsc_type;  /* 1（普通cid查询，即客户端算出来的cid查询），2（社区博客来的cid查询），3（bt来的cid查询）, 4（emule来的cid查询），5 （点播任务的查询）*/
    _u32    _partner_id_size;   
    char        _partner_id[32];    //合作伙伴ID，最长长度为32
    _u32    _product_flag;  
}QUERY_PEER_RES_CMD;











static _u32 seq = 0;
    _int32 ret;
    char* tmp_buf;
    _int32  tmp_len;
    char    http_header[1024] = {0};
    _u32 http_header_len = 1024;
    _u32 encode_len = 0;
    cmd->_protocol_version = PHUB_PROTOCOL_VER;
    cmd->_seq = seq++;
    cmd->_cmd_len = 62 + cmd->_peerid_size + cmd->_cid_size + cmd->_gcid_size + cmd->_partner_id_size;
    cmd->_cmd_type = QUERY_PEER_RES;
    *len = HUB_CMD_HEADER_LEN + cmd->_cmd_len;
    encode_len = (cmd->_cmd_len + HUB_ENCODE_PADDING_LEN) /HUB_ENCODE_PADDING_LEN * HUB_ENCODE_PADDING_LEN + HUB_CMD_HEADER_LEN;
    LOG_DEBUG("build_query_peer_res_cmd,hub_type=%d",device->_hub_type);
    ret = build_http_header(http_header, &http_header_len, encode_len,device->_hub_type);
    CHECK_VALUE(ret);
    ret = sd_malloc(*len + HUB_ENCODE_PADDING_LEN + http_header_len, (void**)buffer);
    if(ret != SUCCESS)
    {
        LOG_DEBUG("build_query_server_res_cmd, malloc failed.");
        CHECK_VALUE(ret);
    }
    sd_memcpy(*buffer, http_header, http_header_len);
    tmp_buf = *buffer + http_header_len;
    tmp_len = (_int32)*len;
    sd_set_int32_to_lt(&tmp_buf, &tmp_len, (_int32)cmd->_protocol_version);
    sd_set_int32_to_lt(&tmp_buf, &tmp_len, (_int32)cmd->_seq);
    sd_set_int32_to_lt(&tmp_buf, &tmp_len, (_int32)cmd->_cmd_len);   
    
    sd_set_int8(&tmp_buf, &tmp_len, (_int8)cmd->_cmd_type);
    sd_set_int32_to_lt(&tmp_buf, &tmp_len, (_int32)cmd->_peerid_size);
    sd_set_bytes(&tmp_buf, &tmp_len, cmd->_peerid, (_int32)cmd->_peerid_size);
    sd_set_int32_to_lt(&tmp_buf, &tmp_len, (_int32)cmd->_cid_size);
    sd_set_bytes(&tmp_buf, &tmp_len, (char*)cmd->_cid, (_int32)cmd->_cid_size);
    sd_set_int64_to_lt(&tmp_buf, &tmp_len, (_int64)cmd->_file_size);
    sd_set_int32_to_lt(&tmp_buf, &tmp_len, (_int32)cmd->_gcid_size);
    sd_set_bytes(&tmp_buf, &tmp_len, (char*)cmd->_gcid, (_int32)cmd->_gcid_size);
    sd_set_int32_to_lt(&tmp_buf, &tmp_len, (_int32)cmd->_peer_capability);
    sd_set_bytes(&tmp_buf, &tmp_len, (char* )&cmd->_internal_ip, sizeof(_u32));
    sd_set_int32_to_lt(&tmp_buf, &tmp_len, (_int32)cmd->_nat_type);
    sd_set_int8(&tmp_buf, &tmp_len, (_int8)cmd->_level_res_num);
    sd_set_int8(&tmp_buf, &tmp_len, (_int8)cmd->_resource_capability);
    sd_set_int32_to_lt(&tmp_buf, &tmp_len, cmd->_server_res_num);
    sd_set_int32_to_lt(&tmp_buf, &tmp_len, (_int32)cmd->_query_times);
    sd_set_int32_to_lt(&tmp_buf, &tmp_len, (_int32)cmd->_p2p_capability);
    sd_set_bytes(&tmp_buf, &tmp_len, (char*)&cmd->_upnp_ip, sizeof(_u32));
    sd_set_int16_to_lt(&tmp_buf, &tmp_len, (_int16)cmd->_upnp_port);
    sd_set_int8(&tmp_buf, &tmp_len, (_int8)cmd->_rsc_type);
    sd_set_int32_to_lt(&tmp_buf, &tmp_len, (_int32)cmd->_partner_id_size);
    sd_set_bytes(&tmp_buf, &tmp_len, cmd->_partner_id, cmd->_partner_id_size);
    sd_set_int32_to_lt(&tmp_buf, &tmp_len, (_int32)cmd->_product_flag);
#ifdef  RES_QUERY_DEBUG
    {
        char test_buffer[1024];
        sd_memset(test_buffer,0,1024);
        str2hex( *buffer + http_header_len, tmp_buf-(*buffer)-http_header_len, test_buffer, 1024);
        LOG_DEBUG( "build_query_peer_res_cmd:buffer[%u]=%s" ,tmp_buf-(*buffer), *buffer);
        printf("\n build_query_peer_res_cmd:buffer[%u]=%s\n" ,tmp_buf-(*buffer), *buffer);
        LOG_DEBUG( "build_query_peer_res_cmd:buffer in hex[%u]=%s" ,tmp_buf-(*buffer)-http_header_len, test_buffer);
        printf("\n build_query_peer_res_cmd:buffer in hex[%u]=%s\n" ,tmp_buf-(*buffer)-http_header_len, test_buffer);
    }
#endif 
#ifndef AMLOS   
    ret = aes_encrypt(*buffer + http_header_len, len);
#else
    ret = aes_encrypt_ext(*buffer + http_header_len, len);
#endif
    if(ret != SUCCESS)
    {
        LOG_ERROR("build_query_peer_res_cmd, but aes_encrypt failed. errcode = %d.", ret);
        sd_free(*buffer);
        *buffer = NULL;
        sd_assert(FALSE);
        return ret;
    }
    sd_assert(tmp_len == 0);
    *len += http_header_len;
    return SUCCESS;