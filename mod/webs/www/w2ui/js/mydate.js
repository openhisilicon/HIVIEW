/*
日期组件,是一个函数.
在input上使用此方法. <input onclick="MyDatePick()" />,需要时间部分: MyDatePick({fmt:datetime})
 */
((win) => {
    //--------------帮助函数-----------帮助函数--------------//
    // 时间格式化函数
    let datefmt = (date, fmtstr) => {
        let format = fmtstr || 'yyyy/MM/dd HH:mm:ss';
        let json = {};
        // 替换时,先替换名字较长的属性,以避免如yyyy被分成两次yy替换,造成错误.故长名字属性在前.
        json.yyyy = date.getFullYear();
        json.yy = json.yyyy.toString().substr(2);
        //
        let m = date.getMonth() + 1;
        json.MM = m > 9 ? m : '0' + m;
        json.M = m;
        //
        let d = date.getDate();
        json.dd = d > 9 ? d : '0' + d;
        json.d = d;
        //
        let h = date.getHours();
        json.HH = h > 9 ? h : '0' + h;
        json.H = h;
        //
        let mi = date.getMinutes();
        json.mm = mi > 9 ? mi : '0' + mi;
        json.m = mi;
        //
        let s = date.getSeconds();
        json.ss = s > 9 ? s : '0' + s;
        json.s = s;
        for (let item in json) {
            format = format.replace(item, json[item]);
        }
        return format;
    };
    // 删除DOM
    let delDom = (doms) => {
        if (!doms.forEach) {
            // 这是单个DOM的情况
            doms.parentNode.removeChild(doms);
            return;
        }
        doms.forEach((dom) => {
            dom.parentNode.removeChild(dom);
        })
    };
    // 获取DOM上的自定义属性的值
    let getAttr = (dom, attrName) => {
        if (!attrName)
            attrName = 'val';
        return dom.attributes[attrName].nodeValue;
    };
    // 设置DOM上的自定义属性值
    let setAttr = (dom, attrVal, attrName) => {
        if (!attrName)
            attrName = 'val';
        dom.setAttribute(attrName, attrVal); // 设置 
    };

    //------datebox类-----------datebox类---------//
    // datebox类名
    let dateboxCls = 'date-box';
    // 触发日期框的INPUT的DOM对象引用
    let inputDOM = null;
    // 日期框DOM对象
    let dateboxDom = null;
    // 日期框配置对象
    let cfg = null;

    // 在input上使用此方法. <input onclick="MyDatePick()" />,需要时间部分: MyDatePick({fmt:datetime})
    let mydate = (config) => {
        let event = window.event || arguments.callee.caller.arguments[0]; // 获取event对象
        event.stopPropagation();
        let input = event.currentTarget;
        // 初始化已选年月日
        initDate(input, config);
        // 生成DOM
        dateboxDom = createDom();
        // 显示
        showDateBox(dateboxDom);
        // 绑定事件
        bindEvent_Show();
    };

    // 初始化:已选年月,保存日期框的INPUT的JQ对象引用
    let initDate = (input, config) => {
        // input的JQ对象
        inputDOM = input;

        // 用inpupt的值初始化时间,为空则默认今天时间.input时间格式只支持 yyyy/MM/dd HH:mm:ss(时间,秒部分可省略)
        let inputval = input.value.trim();
        if (/^[0-9]{4}\/[0-9]{2}\/[0-9]{2}$/.test(inputval)) {
            inputval = inputval + ' 00:00:00';
        } else if (/^[0-9]{4}\/[0-9]{2}\/[0-9]{2} [0-9]{2}:[0-9]{2}$/.test(inputval)) {
            inputval = inputval + ':00';
        } else if (/^[0-9]{4}\/[0-9]{2}\/[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}$/.test(inputval)) {} else {
            inputval = null;
        }
        // console.log(inputval);
        // 不带时间部分的日期串,用parse解后,会有时差.
        let inputDate = Date.parse(inputval);
        let date = isNaN(inputDate) ? new Date((new Date()).setHours(0, 0, 0)) : new Date(inputDate);
        //
        //console.log(date);
        cfg = {};
        cfg.year = date.getFullYear();
        cfg.month = date.getMonth();
        cfg.day = date.getDate();
        cfg.hour = date.getHours();
        cfg.minute = date.getMinutes();
        cfg.second = date.getSeconds();
        // 显示格式为日期('yyyy-MM-dd'),或者日期和时间('yyyy-MM-dd HH:mm:ss')
        cfg.dateFmt = 'yyyy/MM/dd';
        cfg.fmtType = 1;
        if (config && config.fmt == 'datetime') {
            cfg.dateFmt = 'yyyy/MM/dd HH:mm:ss';
            cfg.fmtType = 2;
        }
    };
    // 显示日期框
    let showDateBox = (datedom) => {
        //console.log(datedom);
        // 根据input框的位置显示日期DOM框
        let thisleft = inputDOM.offsetLeft + 'px';
        let thistop = inputDOM.offsetTop + inputDOM.offsetHeight + 'px';
        // 576px以下屏(手机屏) 显示在屏幕中央(css媒体查询设为固定定位了)
        let ww = win.innerWidth;
        if (ww < 576) {
            thisleft = 0;
            thistop = '25vh';
        }

        // 清除可能已有的日期框
        let olddom = document.body.querySelectorAll('.' + dateboxCls);
        delDom(olddom);
        // 显示新的日期框
        datedom.style.left = thisleft;
        datedom.style.top = thistop;
        document.body.append(datedom);

        // 576以上屏,input框要能手动输入,焦点在input框.在手机上使用选择,不使用手输,焦点在日期控件上.
        if (ww < 576) {
            datedom.focus();
        }
    };
    /*=======================================================*
     * DOM生成
     *=======================================================*/
    // 生成整个日期框的DOM.并返回
    let createDom = () => {
        let ymtarea = `<div class="date-area-ymt">${createDom_Year()}${createDom_Month()}${createDom_Today()}</div>`;

        let weekarea = `<div class="date-area-week">${createDom_Week()}</div>`;

        let dayarea = `<div class="date-area-day">${createDom_Day()}</div>`;

        // 时间区域
        let tcarea = '';
        if (cfg.fmtType == 2) {
            tcarea = `<div class="date-area-tc">${createDom_Time()}${createDom_Clear()}${createDom_Ok()}</div>`;
        }
        let datedom = document.createElement('div');
        datedom.classList.add('date-box');
        datedom.tabIndex = -1;
        datedom.innerHTML = `${ymtarea}${weekarea}${dayarea}${tcarea}`;
        return datedom;
    };

    // 1.生成年份区内容 前进,后退,年份 按钮
    let createDom_Year = () => {
        let prevbtn = '<a class="date-btn-prev">&lt;</a>';
        let yearbtn = `<b class="date-btn-year" val="${cfg.year}">${cfg.year}年</b>`;
        let nextbtn = '<a class="date-btn-next">&gt;</a>';
        return `<div class="date-area-year">${prevbtn}${yearbtn}${nextbtn}</div>`;
    };

    // 1.1生成年份下拉选择框. selectedYear:可指定一个年份为已选定
    let createDom_YearSelect = (selectedYear) => {
        let ydoms = '';
        let ylist = domYear_Data();
        if (!selectedYear)
            selectedYear = (new Date()).getFullYear();
        for (let i = 0; i < ylist.length; i++) {
            let isselect = ylist[i] == selectedYear ? "selected" : ""
            ydoms += `<b class="date-option-year ${isselect}" val="${ylist[i]}">${ylist[i]}</b>`;
        }
        let dom = document.createElement('div');
        dom.classList.add('date-select-year');
        dom.innerHTML = ydoms;
        return dom;
    };

    // 2.生成月份区 前进,后退,月份 按钮
    let createDom_Month = () => {
        let prevbtn = '<a class="date-btn-prev">&lt;</a>';
        let monthbtn = `<b class="date-btn-month" val="${cfg.month}">${cfg.month + 1}月</b>`;
        let nextbtn = '<a class="date-btn-next">&gt;</a>';
        return `<div class="date-area-month">${prevbtn}${monthbtn}${nextbtn}</div>`;
    };

    // 2.1生成月份下拉选择框. selectedMonth:可指定一个月份为已选定
    let createDom_MonthSelect = (selectedMonth) => {
        let mdoms = '';
        for (let i = 0; i < 12; i++) {
            let isselect = selectedMonth == i ? "selected" : "";
            mdoms += `<b class="date-option-month ${isselect}" val="${i}">${i + 1}</b>`;
        }
        let dom = document.createElement('div');
        dom.classList.add('date-select-month');
        dom.innerHTML = mdoms;
        return dom;
    };

    // 3.生成星期标题头
    let createDom_Week = () => {
        let weeksdom = '';
        let weeks = ['日', '一', '二', '三', '四', '五', '六'];
        for (let i = 0; i < weeks.length; i++) {
            let isweekend = (i == 0 || i == 6) ? 'date-item-weekend' : '';
            weeksdom += `<b class="date-item-week ${isweekend}">${weeks[i]}</b>`;
        }
        return weeksdom;
    };

    // 4.生成天选项 daylist:日数据.不传则使用选定年月计算出日
    let createDom_Day = (daylist) => {
        let data = daylist || domDay_Data();
        let daydoms = '';
        for (var i = 0; i < data.length; i++) {
            let json = data[i];
            let daydom = '<b class="date-item-day${istoday}${isdayinmonth}${isselected}${isweekend}" year="${yyyy}" month="${MM}" day="${dd}">${dd}</b>';
            json.istoday = json.Istoday ? ' date-item-today' : '';
            json.isselected = json.Isselected ? ' selected' : '';
            json.isdayinmonth = json.Isdayinmonth ? '' : ' date-item-dayoutmonth';
            json.isweekend = json.Isweekend ? ' date-item-weekend' : '';
            //json.exportName = exportName;
            //daydoms += String.DataBind(daydom, json);
            daydoms += daydom.replace(/\${(.+?)\}/g, function(m, key) { return json.hasOwnProperty(key) ? json[key] : '' });
        }
        return daydoms;
    };
    // 5.生成时分秒区域
    let createDom_Time = () => {
        let hour = `<b class="date-btn-time date-btn-hour">${cfg.hour}</b>:`;
        let minute = `<b class="date-btn-time date-btn-minute">${cfg.minute}</b>:`;
        let second = `<b class="date-btn-time date-btn-second">${cfg.second}</b>`;
        return `<div class="date-area-time">${hour}${minute}${second}</div>`;
    };
    // 5.1生成小时选择框
    let createDom_HourSelect = () => {
        let doms = '';
        for (let i = 0; i < 24; i++) {
            doms += `<b class="date-option-hour" val="${i}">${i}</b>`;
        }
        let dom = document.createElement('div');
        dom.classList.add('date-select-hour');
        dom.innerHTML = doms;
        return dom;
    };
    // 5.2生成分钟,秒钟选择框
    let createDom_MinuteSelect = () => {
        let doms = '';
        for (let i = 0; i < 60; i++) {
            doms += `<b class="date-option-minute" val="${i}">${i}</b>`;
        }
        let dom = document.createElement('div');
        dom.classList.add('date-select-minute');
        dom.innerHTML = doms;
        return dom;
    };
    // 5.3生成秒钟选择框
    let createDom_SecondSelect = () => {
        let doms = '';
        for (let i = 0; i < 60; i++) {
            doms += `<b class="date-option-second" val="${i}">${i}</b>`;
        }
        let dom = document.createElement('div');
        dom.classList.add('date-select-second');
        dom.innerHTML = doms;
        return dom;
    };
    // 6.生成今天按钮区域
    let createDom_Today = () => {
        return '<div class="date-area-today"><a class="date-btn-today">今天</a></div>';
    };
    // 7.生成清除按钮区域
    let createDom_Clear = () => {
        return '<div class="date-area-clear"><a class="date-btn-clear">清空</a></div>';
    };
    // 8.生成确定按钮区域 
    let createDom_Ok = () => {
        return '<div class="date-area-ok"><a class="date-btn-ok">确定</a></div>';
    };

    // 根据选定的年,月刷新日(用于当在日期框上操作年,月等会改变年月的动作时)
    // yyyy:指定年,mm:指定月 daysdom:日的父级DOM的JQ对象(.daysrows)
    let resetDaysDom = (yyyy, mm) => {
        // 计算出指定年月的日数据
        let dayslist = domDay_Data(yyyy, mm);
        // 生成天DOM
        let daysdom = createDom_Day(dayslist);
        // 更新天DOM
        dateboxDom.querySelector('.date-area-day').innerHTML = daysdom;
        // 事件绑定
        bindEvent_DaySelected();
    };

    /*----------------为DOM提供的数据,年份 日-----------为DOM提供的数据,年份 日-------- */
    // 根据已选年计算年份选项
    let domYear_Data = () => {
        // 年份选择范围固定在[1900-2100]
        let data = [];
        for (let i = 1900; i < 2101; i++) {
            data.push(i);
        }
        return data;
    };

    // 根据已选年月或者传入指定年月,计算日的起始和结束
    // 日(天)总共六行七列42个,含已选年月所有日, 前推至最近的周日, 后推至最近或次近的周六
    let domDay_Data = (yyyy, mm) => {
        // 指定年 超范围则设为当天年
        let seledY = parseInt(yyyy) || cfg.year;
        // 指定月 超范围设为当天月
        let seledM = parseInt(mm) || cfg.month;

        // 指定年月的起止日(1~xx号)
        let startDay = new Date(seledY, seledM, 1);
        //let endDay = new Date(seledY, seledM + 1, 0);

        // 日期起点为指定年月的1号前推到最近的周日,终点为该月最后一天后推到最近的周六
        startDay.setDate(1 - startDay.getDay());
        //endDay.setDate(endDay.getDate() + (6 - endDay.getDay()));
        // 当天日期
        let todaystr = datefmt(new Date(), 'yyyyMMdd');
        let daylist = [];
        for (let i = 0; i < 42; i++) {
            let json = {};
            json.yyyy = startDay.getFullYear();
            json.MM = startDay.getMonth();
            json.dd = startDay.getDate();
            // 日是否属于指定年月中的日
            json.Isdayinmonth = json.MM == seledM;
            // 日是否为今天 
            json.Istoday = datefmt(startDay, 'yyyyMMdd') == todaystr;
            // 日是否选定(等于文本框中已选日)
            json.Isselected =
                (json.yyyy == cfg.year && json.MM == cfg.month &&
                    json.dd == cfg.day);
            // 这天是否为周六日(这里未真正判断,而是根据位置判断,每七天为一行,行首周日行尾周六)
            json.Isweekend = (i % 7 == 0 || (i + 1) % 7 == 0);
            //
            startDay.setDate(json.dd + 1);
            daylist.push(json);
        }
        //console.log(daylist);
        return daylist;
    };

    /*============================================================*
     * 事件方法:年,月的前进后退按钮,年月选择按钮,今天按钮
     *============================================================*/
    // 控件显示后,要绑定控件的基础事件.
    let bindEvent_Show = () => {
        bindEvent_DateBox();
        bindEvent_YearBtn();
        bindEvent_MonthBtn();
        bindEvent_YearMonthPrevNext();
        bindEvent_TodayBtn();

        bindEvent_DaySelected();
        // 小时,分钟,秒,取消,确定,按钮在有时分秒格式时才有
        if (cfg.fmtType == 2) {
            bindEvent_HourBtn();
            bindEvent_MinBtn();
            bindEvent_SecBtn();
            bindEvent_ClearBtn();
            bindEvent_OkBtn();
        }
    };

    let bindEvent_DateBox = () => {
        // 点击日期控件以内区域,阻止冒泡到根
        dateboxDom.onclick = (event) => {
            event.stopPropagation();
            // 点击空白位置时,关闭已经打开的年,月,日,时,分,秒的选择框.需要在子元素上取消冒泡
            let partSelectDom = dateboxDom.querySelectorAll('[class^=date-select]');
            delDom(partSelectDom);
        };
    };
    let bindEvent_YearBtn = () => {
        // 点击年按钮 显示年选择框
        dateboxDom.querySelector('.date-btn-year').onclick = (event) => {
            event.stopPropagation();
            let thisobj = event.currentTarget;
            //
            let seledY = getAttr(thisobj);
            // 年份选择框 .date-select-year
            let yearopsbox = thisobj.parentElement.querySelector('.date-select-year');
            // 如果已经显示则关闭
            if (yearopsbox) {
                delDom(yearopsbox);
                return;
            }
            // 先关闭其它弹出窗
            let otherDoms = dateboxDom.querySelectorAll('[class^=date-select]');
            delDom(otherDoms);
            // 生成年份选择框,填充到年份选择框中
            let yearSelectDom = createDom_YearSelect(seledY);
            thisobj.parentElement.append(yearSelectDom);
            // 定位已选年份到滚动框的中间(视口可见范围内)
            let yseled = yearSelectDom.querySelector('.selected');

            // 计算这个年份选项离父框的TOP值,然后滚动条滚动这个值-父框高/2
            let scrollval = yseled.offsetTop - yearSelectDom.clientHeight / 2;
            yearSelectDom.scrollTo(0, scrollval);
            // 绑定年份选择点击事件
            bindEvent_YearSelected();
        };
    };
    let bindEvent_MonthBtn = () => {
        // 点击月按钮 显示月选择框
        dateboxDom.querySelector('.date-btn-month').onclick = (event) => {
            event.stopPropagation();
            let thisobj = event.currentTarget;
            //
            let seledM = getAttr(thisobj);
            let monthsops = thisobj.parentElement.querySelector('.date-select-month');
            // 如果已经显示则关闭
            if (monthsops) {
                delDom(monthsops);
                return;
            }
            // 先关闭其它弹出窗
            let otherDoms = dateboxDom.querySelectorAll('[class^=date-select]');
            delDom(otherDoms);
            //
            thisobj.parentElement.append(createDom_MonthSelect(seledM));
            // 绑定月分选项点击事件
            bindEvent_MonthSelected();
        };
    };
    let bindEvent_YearSelected = () => {
        // 点击年份选项 选定一个年份 
        dateboxDom.querySelectorAll('.date-option-year').forEach((item) => {
            item.onclick = (event) => {
                event.stopPropagation();
                let thisobj = event.currentTarget;
                // 
                // 所选年份值
                let y = getAttr(thisobj);
                // 更新年份按钮显示值
                let yearBtn = dateboxDom.querySelector('.date-btn-year');
                setAttr(yearBtn, y);
                yearBtn.innerHTML = y + '年';
                // 关闭年份选择框
                delDom(thisobj.parentElement)
                    // 刷新 日
                let m = getAttr(dateboxDom.querySelector('.date-btn-month'));
                resetDaysDom(y, m);
            };
        });
    };
    let bindEvent_MonthSelected = () => {
        // 点击月份选项 选定一个月份
        dateboxDom.querySelectorAll('.date-option-month').forEach((item) => {
            item.onclick = (event) => {
                event.stopPropagation();
                let thisobj = event.currentTarget;
                // 
                // 所选月份值
                let m = parseInt(getAttr(thisobj));
                let monthBtn = dateboxDom.querySelector('.date-btn-month');
                setAttr(monthBtn, m);
                monthBtn.innerHTML = (m + 1) + '月';
                // 关闭月份选择框
                delDom(thisobj.parentElement);
                // 刷新 日
                let y = getAttr(dateboxDom.querySelector('.date-btn-year'));
                resetDaysDom(y, m);
            };
        });
    };
    let bindEvent_YearMonthPrevNext = () => {
        // 点击年份,月份的前进和后退按钮 btntype:1=年按钮,2=月按钮. dir:1=前进,2=后退
        dateboxDom.querySelectorAll('.date-btn-prev,.date-btn-next').forEach((item) => {
            item.onclick = (event) => {
                event.stopPropagation();
                let thisobj = event.currentTarget;
                //
                let btntype = thisobj.parentElement.classList.contains('date-area-year') ? 1 : 2;
                let dir = thisobj.classList.contains('date-btn-next') ? 1 : 2;
                //
                let ybtn = dateboxDom.querySelector('.date-btn-year');
                let mbtn = dateboxDom.querySelector('.date-btn-month');
                let y = parseInt(getAttr(ybtn));
                let m = parseInt(getAttr(mbtn));
                // 计算并刷新年或月按钮值 年份前进后退值[1-9999]
                if (btntype == 1) {
                    y = dir == 1 ? y + 1 : y - 1;
                    if (y < 1) y = 9999;
                    else if (y > 9999) y = 1;
                } else if (btntype == 2) {
                    m = dir == 1 ? m + 1 : m - 1;
                    if (m < 0) {
                        m = 11;
                        // 年往后退一年,如果为1年,则不变
                        if (y > 1)
                            y = y - 1;
                    } else if (m > 11) {
                        m = 0;
                        // 年往前进一年,如果为9999年,则不变
                        if (y < 9999)
                            y = y + 1;
                    }
                }
                setAttr(ybtn, y);
                ybtn.innerHTML = y + '年';
                setAttr(mbtn, m);
                mbtn.innerHTML = (m + 1) + '月';
                // 刷新日
                //console.log(y+'----'+m);
                resetDaysDom(y, m);
            };
        });
    };
    let bindEvent_TodayBtn = () => {
        // 点击今天按钮 设置今天日期到input框
        dateboxDom.querySelector('.date-btn-today').onclick = (event) => {
            event.stopPropagation();
            let thisobj = event.currentTarget;
            //
            let today = new Date(new Date().toLocaleDateString());
            inputDOM.value = datefmt(today, cfg.dateFmt);
            //
            mydate.close();
        };
    };
    let bindEvent_HourBtn = () => {
        // 点击小时按钮 显示小时选择框
        dateboxDom.querySelector('.date-btn-hour').onclick = (event) => {
            event.stopPropagation();
            let thisobj = event.currentTarget;
            //
            let hourselecct = thisobj.parentElement.querySelector('.date-select-hour');
            // 点击小时按钮时,弹出小时选择框,同时,按钮加上打开样式,以表示当前选择的是小时
            // 添加样式时,先取消其按钮的打开样式,打开后,再给自己加上打开样式
            let otherBtns = thisobj.parentElement.querySelectorAll('.date-btn-time');
            otherBtns.forEach((item) => { item.classList.remove('open') });
            // 如果已经是打开状态则关闭
            if (hourselecct) {
                delDom(hourselecct);
                return;
            }
            // 先关闭其它弹出窗
            let otherdoms = dateboxDom.querySelectorAll('[class^=date-select]');
            delDom(otherdoms);
            // 显示小时选择框
            thisobj.parentElement.append(createDom_HourSelect());
            thisobj.classList.add('open');
            // 绑定小时选项点击事件
            bindEvent_HourSelected();
        };
    };
    let bindEvent_MinBtn = () => {
        // 点击分钟按钮 显示分钟选择框
        dateboxDom.querySelector('.date-btn-minute').onclick = (event) => {
            event.stopPropagation();
            let thisobj = event.currentTarget;
            //
            let minselecct = thisobj.parentElement.querySelector('.date-select-minute');
            // 点击时分秒下拉框按钮时,先取消其按钮的打开样式,打开后,再给自己加上打开样式
            let otherBtns = thisobj.parentElement.querySelectorAll('.date-btn-time');
            otherBtns.forEach((item) => { item.classList.remove('open') });

            // 如果已经显示则关闭
            if (minselecct) {
                delDom(minselecct);
                return;
            }
            // 先关闭其它弹出窗
            let otherdoms = dateboxDom.querySelectorAll('[class^=date-select]');
            delDom(otherdoms);
            thisobj.parentElement.append(createDom_MinuteSelect());
            thisobj.classList.add('open');
            // 绑定分钟选项点击事件
            bindEvent_MinSelected();
        };
    };
    let bindEvent_SecBtn = () => {
        // 点击秒钟按钮 显示秒钟选择框
        dateboxDom.querySelector('.date-btn-second').onclick = (event) => {
            event.stopPropagation();
            let thisobj = event.currentTarget;
            //
            let secselecct = thisobj.parentElement.querySelector('.date-select-second');
            // 点击时分秒下拉框按钮时,先取消其按钮的打开样式,打开后,再给自己加上打开样式
            let otherBtns = thisobj.parentElement.querySelectorAll('.date-btn-time');
            otherBtns.forEach((item) => { item.classList.remove('open') });
            // 如果已经显示则关闭
            if (secselecct) {
                delDom(secselecct);
                return;
            }
            // 先关闭其它弹出窗
            let otherdoms = dateboxDom.querySelectorAll('[class^=date-select]');
            delDom(otherdoms);
            thisobj.parentElement.append(createDom_SecondSelect());
            thisobj.classList.add('open');
            // 绑定秒钟选项点击事件
            bindEvent_SecSelected();
        }
    };
    let bindEvent_HourSelected = () => {
        // 选择小时 修改小时按钮显示值
        dateboxDom.querySelectorAll('.date-option-hour').forEach((item) => {
            item.onclick = (event) => {
                event.stopPropagation();
                let thisobj = event.currentTarget;
                //
                let h = getAttr(thisobj);
                let btnHour = dateboxDom.querySelector('.date-btn-hour');
                btnHour.innerHTML = h;
                cfg.hour = h;
                //
                delDom(thisobj.parentElement);
            };
        });
    };
    let bindEvent_MinSelected = () => {
        // 选择分钟 修改按钮显示值
        dateboxDom.querySelectorAll('.date-option-minute').forEach((item) => {
            item.onclick = (event) => {
                event.stopPropagation();
                let thisobj = event.currentTarget;
                //
                let m = getAttr(thisobj);
                dateboxDom.querySelector('.date-btn-minute').innerHTML = m;
                cfg.minute = m;
                //
                delDom(thisobj.parentElement);
            };
        })
    };
    let bindEvent_SecSelected = () => {
        // 选择秒钟 修改按钮显示值
        dateboxDom.querySelectorAll('.date-option-second').forEach((item) => {
            item.onclick = (event) => {
                event.stopPropagation();
                let thisobj = event.currentTarget;
                //
                let s = getAttr(thisobj);
                dateboxDom.querySelector('.date-btn-second').innerHTML = s;
                cfg.second = s;
                //
                delDom(thisobj.parentElement);
            };
        })
    };
    let bindEvent_DaySelected = () => {
        // 选择天 设置这天日期到Input框
        dateboxDom.querySelectorAll('.date-item-day').forEach((item) => {
            item.onclick = (event) => {
                event.stopPropagation();
                let thisobj = event.currentTarget;
                //
                let date = new Date(getAttr(thisobj, 'year'), getAttr(thisobj, 'month'), getAttr(thisobj, 'day'), cfg.hour, cfg.minute, cfg.second);
                inputDOM.value = datefmt(date, cfg.dateFmt);
                //
                mydate.close();
            };
        });
    };
    let bindEvent_ClearBtn = () => {
        // 点击清空
        dateboxDom.querySelector('.date-btn-clear').onclick = (event) => {
            event.stopPropagation();
            // let thisobj = event.currentTarget;
            //
            inputDOM.value = '';
            mydate.close();
        };
    };
    let bindEvent_OkBtn = () => {
        // 点击确定按钮
        dateboxDom.querySelector('.date-btn-ok').onclick = (event) => {
            event.stopPropagation();
            // let thisobj = event.currentTarget;
            //
            // 找到选中的日 设置到Input框 如果没有选中的日,使用当前设置日期
            let seledDay = dateboxDom.querySelector('.date-item-day.selected');
            let dateStr = datefmt(new Date(cfg.year, cfg.month, cfg.day, cfg.hour, cfg.minute, cfg.second), cfg.dateFmt);
            if (seledDay) {
                let d = new Date(getAttr(seledDay, 'year'), getAttr(seledDay, 'month'), getAttr(seledDay, 'day'), cfg.hour, cfg.minute, cfg.second);
                dateStr = datefmt(d, cfg.dateFmt);
            }

            inputDOM.value = dateStr;
            //
            mydate.close();
        }
    };

    // 关闭日期框
    mydate.close = () => {
        dateboxDom = null;
        inputDOM = null;
        cfg = null;
        let datedoms = document.body.querySelectorAll('.' + dateboxCls);
        delDom(datedoms);
    };

    // 点击日期控件以外区域,关闭控件. 
    document.onclick = () => {
        mydate.close();
    };
    // 日期函数名,可在引修改
    win.MyDatePick = mydate;
})(window);